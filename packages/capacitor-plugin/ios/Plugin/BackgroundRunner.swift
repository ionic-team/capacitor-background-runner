import Foundation
import Capacitor
import BackgroundTasks
import JavaScriptCore

public class BackgroundRunner {
    public static let shared = BackgroundRunner()
    public var config: RunnerConfig?

    private var runner = Runner()

    public init() {
        do {
            config = try self.loadRunnerConfig()
        } catch {
            print("could not initialize BackgroundRunner: \(error)")
        }
    }

    public func scheduleBackgroundTasks() throws {
        guard let config = config else {
            throw BackgroundRunnerPluginError.noRunnerConfig
        }

        if !config.autoSchedule {
            return
        }

        guard let interval = config.interval else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "cannot register background task without a configured interval")
        }

        let request = BGAppRefreshTaskRequest(identifier: config.label)
        request.earliestBeginDate = Date(timeIntervalSinceNow: Double(interval) * 60)

        print("Scheduling \(config.label)")

        try BGTaskScheduler.shared.submit(request)
    }

    public func registerBackgroundTask() throws {
        guard let config = config else {
            throw BackgroundRunnerPluginError.noRunnerConfig
        }

        BGTaskScheduler.shared.register(forTaskWithIdentifier: config.label, using: nil) { task in
            do {
                task.expirationHandler = {
                    print("task timed out")
                }

                _ = try BackgroundRunner.shared.execute(config: config)

                task.setTaskCompleted(success: true)

                if config.repeats {
                    try self.scheduleBackgroundTasks()
                }
            } catch {
                print("background task error: \(error)")
                task.setTaskCompleted(success: false)
            }
        }
    }

    // swiftlint:disable:next cyclomatic_complexity function_body_length
    public func execute(config: RunnerConfig, inputArgs: [String: Any]? = nil, callbackId: String? = nil) throws -> [String: Any]? {
        do {
            let context = try initContext(config: config, callbackId: callbackId)

            let waitGroup = DispatchGroup()

            var result: [String: Any]?
            var rejectionErr: JSError?

            let resolveFunc: @convention(block)(JavaScriptCore.JSValue) -> Void = { returnObj in
                if !returnObj.isUndefined && !returnObj.isNull {
                    var dict: [String: Any] = [:]

                    returnObj.toDictionary().forEach { key, value in
                        dict[String(describing: key)] = value
                    }

                    result = dict
                }

                waitGroup.leave()
                return
            }

            let rejectFunc: @convention(block)(JavaScriptCore.JSValue) -> Void = { rejectObj in
                var rejectionTitle = "Error"
                var rejectionMessage = ""

                if !rejectObj.isUndefined && !rejectObj.isNull {
                    if let errStr = rejectObj.toString() {
                        let split = errStr.split(separator: ":", maxSplits: 1)
                        if split.count == 0 {
                            rejectionMessage = String(split.first ?? "")
                        } else {
                            rejectionTitle = String(split.first ?? "Error")
                            rejectionMessage = String(split.last ?? "")
                        }
                    }

                    if rejectObj.isObject, let errDict = rejectObj.toDictionary() {
                        if let errTitle = errDict["name"] as? String {
                            rejectionTitle = errTitle
                        }

                        if let errMsg = errDict["message"] as? String {
                            rejectionMessage = errMsg
                        }
                    }
                }

                rejectionErr = JSError(message: "\(rejectionTitle): \(rejectionMessage)")

                waitGroup.leave()
                return
            }

            var args: [String: Any] = [:]

            var callbacks: [String: Any] = [:]
            callbacks["__cbr::resolve"] = resolveFunc
            callbacks["__cbr::reject"] = rejectFunc

            args["callbacks"] = callbacks
            args["dataArgs"] = inputArgs

            waitGroup.enter()

            try context.dispatchEvent(event: config.event, details: args)

            waitGroup.wait()

            if let rejection = rejectionErr {
                throw EngineError.jsException(details: rejection.message)
            }

            return result
        } catch {
            print("error executing task: \(error)")
            throw error
        }
    }

    public func dispatchEvent(event: String, inputArgs: [String: Any]?, callbackId: String? = nil) throws {
        if let config = config {
            let waitGroup = DispatchGroup()
            waitGroup.enter()

            var err: Error?

            // swiftlint:disable:next unowned_variable_capture
            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    _ = try self.execute(config: config, inputArgs: inputArgs, callbackId: callbackId)
                } catch {
                    err = error
                    print("[\(config.label)]: \(error)")
                }

                waitGroup.leave()
            }

            waitGroup.wait()

            if let err = err {
                throw err
            }
        }
    }

    private func loadRunnerConfig() throws -> RunnerConfig? {
        guard let capacitorConfigFileURL = Bundle.main.url(forResource: "capacitor.config.json", withExtension: nil) else {
            throw BackgroundRunnerPluginError.runnerError(reason: "capacitor.config.json file not found")
        }

        let capacitorConfigFileData = try Data(contentsOf: capacitorConfigFileURL)

        if let capConfig = JSTypes.coerceDictionaryToJSObject(try JSONSerialization.jsonObject(with: capacitorConfigFileData) as? [String: Any]) {
            if let jsonConfig = capConfig[keyPath: "plugins.BackgroundRunner"] as? JSObject {
                return try RunnerConfig(from: jsonConfig)
            }
        }

        return nil
    }

    private func initContext(config: RunnerConfig, callbackId: String?) throws -> Context {
        guard let srcFileURL = Bundle.main.url(forResource: config.src, withExtension: nil, subdirectory: "public") else {
            throw BackgroundRunnerPluginError.runnerError(reason: "source file not found")
        }

        var contextName = config.label
        if let callbackId = callbackId {
            contextName = "\(contextName)-\(callbackId)"
        }

        let srcFile = try String(contentsOf: srcFileURL)

        let context = try runner.createContext(name: contextName)

        context.setupCapacitorAPI()

        _ = try context.execute(code: srcFile)

        return context
    }
}
