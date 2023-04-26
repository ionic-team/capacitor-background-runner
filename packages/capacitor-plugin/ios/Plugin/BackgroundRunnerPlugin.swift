import Foundation
import Capacitor
import BackgroundTasks
import JavaScriptCore

/**
 * Please read the Capacitor iOS Plugin Development Guide
 * here: https://capacitorjs.com/docs/plugins/ios
 */
@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin {
    private var runnerConfigs: [String: RunnerConfig] = [:]
    
    override public func load() {        
        NotificationCenter.default.addObserver(self, selector: #selector(didEnterBackground), name: UIApplication.didEnterBackgroundNotification, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(didFinishLaunching), name: UIApplication.didFinishLaunchingNotification, object: nil)
        
        do {
            if let jsonRunnerConfigs = self.getConfig().getArray("runners") {
                for index in 0...jsonRunnerConfigs.count - 1 {
                    if let jsonConfig = jsonRunnerConfigs[index] as? JSObject {
                        let runnerConfig = try RunnerConfig(from: jsonConfig)
                        self.runnerConfigs[runnerConfig.label] = runnerConfig
                    }
                }
            }
        } catch {
            print(error.localizedDescription)
        }
    }
    
    @objc func dispatchEvent(_ call: CAPPluginCall) {
        do {
            guard let runnerLabel = call.getString("label") else {
                throw BackgroundRunnerPluginError.invalidArgument(reason: "label is missing or invalid")
            }
            
            guard let runnerEvent = call.getString("event") else {
                throw BackgroundRunnerPluginError.invalidArgument(reason: "event is missing or invalid")
            }
            
            let details = call.getObject("details", JSObject())
            
            guard let config = self.runnerConfigs[runnerLabel] else {
                throw BackgroundRunnerPluginError.runnerError(reason: "no runner config found for label")
            }
            
            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    if let result = try self.executeRunner(config: config, args: details as [String: Any], overrideEvent: runnerEvent, task: nil) {
                        call.resolve(result)
                    } else {
                        call.resolve()
                    }
                } catch {
                    call.reject("\(error)")
                }
            }
        } catch {
            call.reject("\(error)")
        }
    }
    
    @objc func registerBackgroundTask(_ call: CAPPluginCall) {
        do {
            guard let newConfigJSON = call.getObject("runner") else {
                throw BackgroundRunnerPluginError.invalidArgument(reason: "runner is missing or invalid")
            }
            
            var newConfig = try RunnerConfig(from: newConfigJSON)
            newConfig.autoStart = true
            
            runnerConfigs[newConfig.label] = newConfig
            
            call.resolve()
        } catch {
            call.reject("\(error)")
        }
    }
    
    @objc private func didFinishLaunching() {
        self.registerRunnerTasks()
    }
    
    @objc private func didEnterBackground()  {
        self.runnerConfigs.forEach { _, config in
            if config.autoStart {
                self.scheduleRunnerTask(runnerConfig: config)
            }
        }
    }
    
    private func registerRunnerTasks() {
        self.runnerConfigs.forEach { _, runnerConfig in
            BGTaskScheduler.shared.register(forTaskWithIdentifier: runnerConfig.label, using: nil) { task in
                _ = try? self.executeRunner(config: runnerConfig, overrideEvent: nil, task: task as? BGAppRefreshTask)
            }
        }
    }
    
    private func scheduleRunnerTask(runnerConfig: RunnerConfig) {
        let request = BGAppRefreshTaskRequest(identifier: runnerConfig.label)
        request.earliestBeginDate = Date(timeIntervalSinceNow: Double(runnerConfig.interval) * 60)
        
        do {
            try BGTaskScheduler.shared.submit(request)
        } catch {
            print("Could not schedule app refresh: \(error)")
        }
    }
    
    private func executeRunner(config: RunnerConfig, args: [String: Any]? = nil, overrideEvent: String? = nil ,task: BGAppRefreshTask? = nil) throws -> [String: Any]? {
        print("successfully executing task")
        
        if config.repeats && task != nil {
            self.scheduleRunnerTask(runnerConfig: config)
        }
        
        do {
            guard let srcFileURL = Bundle.main.url(forResource: config.src, withExtension: nil, subdirectory: "public") else {
                throw BackgroundRunnerPluginError.runnerError(reason: "source file not found")
            }
            
            var result: [String: Any]? = nil
            
            let waitGroup = DispatchGroup()
                    
            let completedFunc: @convention(block)(JavaScriptCore.JSValue) -> Void = { returnObj in
                if (!returnObj.isUndefined && !returnObj.isNull) {
                    var dict: [String: Any] = [:]
                    
                    returnObj.toDictionary().forEach { key, value in
                        dict[String(describing: key)] = value
                    }
                    
                    result = dict
                }
                
                waitGroup.leave()
                return
            }
            
            var argsWithCallback = args
            
            if (argsWithCallback == nil) {
                argsWithCallback = [:]
            }
            
            argsWithCallback?["__ebr::completed"] = completedFunc

            
            let srcFile = try String(contentsOf: srcFileURL)
            
            let runner = Runner()
            let context = try runner.createContext(name: config.label)
            
            _ = try context.execute(code: srcFile)
            
            waitGroup.enter()
            
            if let event = overrideEvent {
                try context.dispatchEvent(event: event, details: argsWithCallback)
            } else {
                try context.dispatchEvent(event: config.event, details: argsWithCallback)
            }
            
            waitGroup.wait()
            
            if let task = task {
                task.setTaskCompleted(success: true)
            }
            
            return result
        } catch {
            if let task = task {
                print(error)
                task.setTaskCompleted(success: false)
            }
            
            throw error
        }
    }
}
