//
//  BackgroundRunner.swift
//  IonicEnterpriseBackgroundRunner
//
//  Created by Joseph Pender on 5/18/23.
//

import Foundation
import Capacitor
import BackgroundTasks
import JavaScriptCore

public class BackgroundRunner {
    static let shared = BackgroundRunner()
    
    private var runner: Runner? = nil
    private var configs: [String: RunnerConfig] = [:]
    private var contexts: [String: Context] = [:]
    
    private var started: Bool {
        return runner != nil
    }
    
    public init() {
        do {
            self.configs = try self.loadRunnerConfig()
        } catch {
            print("could not initialize BackgroundRunner: \(error)")
        }
    }
    
    public func registerBackgroundTasks() {
        configs.forEach { _, config in
            BGTaskScheduler.shared.register(forTaskWithIdentifier: config.label, using: nil) { task in
                _ = try? BackgroundRunner.shared.execute(config: config, task: (task as! BGAppRefreshTask))
            }
        }
    }
    
    public func scheduleBackgroundTasks() {
        configs.forEach { (label: String, config: RunnerConfig) in
            if config.autoStart {
                scheduleBackgroundTask(config: config)
            }
        }
    }
    
    public func start() throws {
        print("starting runner and loading contexts...")
        self.runner = Runner()
    
        // create and load runner and contexts
        try configs.forEach { _, config in
            contexts[config.label] = try initContext(config: config)
        }
    }
    
    public func stop() {
        print("...stopping runner and removing all contexts")
        guard let runner = runner else {
            return
        }
        
        contexts.forEach({ (name: String, context: Context) in
            runner.destroyContext(name: name)
        })
        
        self.contexts.removeAll()
        
        self.runner = nil
    }
    
    public func getConfig(name: String) -> RunnerConfig? {
        return configs[name]
    }
    
    public func execute(config: RunnerConfig, task: BGAppRefreshTask? = nil, inputArgs: [String: Any]? = nil, eventOverride: String? = nil) throws -> [String: Any]? {
        do {
            defer {
                if task != nil && config.repeats {
                    self.scheduleBackgroundTask(config: config)
                }
                
                stop()
            }
            
            if !started {
                try self.start()
            }
            
            guard let context = contexts[config.label] else {
                throw BackgroundRunnerPluginError.runnerError(reason: "could not find context for label")
            }
            
            var inputArgsWithCallback = inputArgs
            
            if (inputArgsWithCallback == nil) {
                inputArgsWithCallback = [:]
            }
            
            var event = config.event
            
            if let eventOverride = eventOverride {
                event = eventOverride
            }
            
            let waitGroup = DispatchGroup()
            
            var result: [String: Any]? = nil
                    
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
            
            inputArgsWithCallback?["__ebr::completed"] = completedFunc
            
            if let task = task {
                task.expirationHandler = {
                    print("task timed out")
                    waitGroup.leave()
                }
            }
            
            waitGroup.enter()
            
            try context.dispatchEvent(event: event, details: inputArgsWithCallback)
            
            waitGroup.wait()
            
            if let task = task {
                task.setTaskCompleted(success: true)
            }
            
            return result
        } catch {
            print("error executing task: \(error)")
            if let task = task {
                task.setTaskCompleted(success: false)
            }
            
            throw error
        }
    }
    
    private func loadRunnerConfig() throws -> [String: RunnerConfig] {
        var runnerConfigs: [String: RunnerConfig] = [:]
        
        guard let capacitorConfigFileURL = Bundle.main.url(forResource: "capacitor.config.json", withExtension: nil) else {
            throw BackgroundRunnerPluginError.runnerError(reason: "capacitor.config.json file not found")
        }
        
        let capacitorConfigFileData = try Data(contentsOf: capacitorConfigFileURL)
        
        if let capConfig = JSTypes.coerceDictionaryToJSObject(try JSONSerialization.jsonObject(with: capacitorConfigFileData) as? [String: Any]) {
            if let runners = capConfig[keyPath: "plugins.BackgroundRunner.runners"] as? JSArray {
                for index in 0...runners.count - 1 {
                    if let jsonConfig = runners[index] as? JSObject {
                        let runnerConfig = try RunnerConfig(from: jsonConfig)
                        runnerConfigs[runnerConfig.label] = runnerConfig
                    }
                }
            }
        }
        
        return runnerConfigs
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
    
    private func scheduleBackgroundTask(config: RunnerConfig) {
        let request = BGAppRefreshTaskRequest(identifier: config.label)
        request.earliestBeginDate = Date(timeIntervalSinceNow: Double(config.interval) * 60)
        
        do {
            print("Scheduling \(config.label)")
            
            try BGTaskScheduler.shared.submit(request)
        } catch {
            print("Could not schedule app refresh: \(error)")
        }
    }
}
