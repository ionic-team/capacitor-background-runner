import Foundation
import Capacitor
import BackgroundTasks
import JavaScriptCore

public class BackgroundRunner {
    public static let shared = BackgroundRunner()

    private var runner: Runner?
    private var config: RunnerConfig?

    private var context: Context?

    private var started: Bool {
        return runner != nil && context != nil
    }

    public init() {
        do {
            config = try self.loadRunnerConfig()
        } catch {
            print("could not initialize BackgroundRunner: \(error)")
        }
    }

    public func registerBackgroundTask() {
        guard let config = config else {
            print("no runner to register")
            return
        }

        if config.event == nil {
            print("cannot register background task without a event to call")
            return
        }

        BGTaskScheduler.shared.register(forTaskWithIdentifier: config.label, using: nil) { task in
            do {
                task.expirationHandler = {
                    print("task timed out")
                }

                guard let event = config.event else {
                    throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner event is missing or invalid")
                }

                _ = try BackgroundRunner.shared.execute(config: config, event: event)

                task.setTaskCompleted(success: true)
            } catch {
                print("background task error: \(error)")
                task.setTaskCompleted(success: false)
            }
        }
    }

    public func scheduleBackgroundTasks() {
        guard let config = config else {
            return
        }

        guard let interval = config.interval else {
            print("cannot register background task without a configured interval")
            return
        }

        let request = BGAppRefreshTaskRequest(identifier: config.label)
        request.earliestBeginDate = Date(timeIntervalSinceNow: Double(interval) * 60)

        do {
            print("Scheduling \(config.label)")

            try BGTaskScheduler.shared.submit(request)
        } catch {
            print("Could not schedule app refresh: \(error)")
        }
    }

    public func start() throws {
        print("starting runner and loading contexts...")

        guard let config = config else {
            print("...no runner config to start")
            return
        }

        self.runner = Runner()
        context = try initContext(config: config)

    }

    public func stop() {
        print("...stopping runner and removing all contexts")
        guard let runner = runner, let config = config else {
            return
        }

        runner.destroyContext(name: config.label)

        context = nil
        self.runner = nil
    }

    public func getConfig() -> RunnerConfig? {
        return config
    }

    public func dispatchEvent(event: String, inputArgs: [String: Any]?) throws {
        if !started {
            try start()
        }

        if let config = config {
            let waitGroup = DispatchGroup()
            waitGroup.enter()

            var err: Error?

            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    _ = try self.execute(config: config, event: event, inputArgs: inputArgs)
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

    public func execute(config: RunnerConfig, event: String, inputArgs: [String: Any]? = nil) throws -> [String: Any]? {
        do {
            if !started {
                try self.start()
            }

            guard let context = context else {
                throw BackgroundRunnerPluginError.runnerError(reason: "no loaded context for config")
            }

            var inputArgsWithCallback = inputArgs

            if inputArgsWithCallback == nil {
                inputArgsWithCallback = [:]
            }

            let waitGroup = DispatchGroup()

            var result: [String: Any]?

            let completedFunc: @convention(block)(JavaScriptCore.JSValue) -> Void = { returnObj in
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

            inputArgsWithCallback?["__ebr::completed"] = completedFunc

            waitGroup.enter()

            try context.dispatchEvent(event: event, details: inputArgsWithCallback)

            waitGroup.wait()

            return result
        } catch {
            print("error executing task: \(error)")
            throw error
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

    private func initContext(config: RunnerConfig) throws -> Context {
        guard let runner = runner else {
            throw BackgroundRunnerPluginError.runnerError(reason: "runner is not initialized")
        }

        guard let srcFileURL = Bundle.main.url(forResource: config.src, withExtension: nil, subdirectory: "public") else {
            throw BackgroundRunnerPluginError.runnerError(reason: "source file not found")
        }

        let srcFile = try String(contentsOf: srcFileURL)

        let context = try runner.createContext(name: config.label)

        context.setupCapacitorAPI()

        _ = try context.execute(code: srcFile)

        return context
    }
}
