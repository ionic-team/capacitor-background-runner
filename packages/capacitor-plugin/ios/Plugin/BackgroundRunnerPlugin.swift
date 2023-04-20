import Foundation
import Capacitor
import BackgroundTasks

/**
 * Please read the Capacitor iOS Plugin Development Guide
 * here: https://capacitorjs.com/docs/plugins/ios
 */
@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin {
    private var runnerConfigs: [RunnerConfig] = []
    
    override public func load() {
        NotificationCenter.default.addObserver(self, selector: #selector(didEnterBackground), name: UIApplication.didEnterBackgroundNotification, object: nil)
        NotificationCenter.default.addObserver(self, selector: #selector(didFinishLaunching), name: UIApplication.didFinishLaunchingNotification, object: nil)
        
        do {
            if let jsonRunnerConfigs = self.getConfig().getArray("runners") {
                for index in 0...jsonRunnerConfigs.count - 1 {
                    if let jsonConfig = jsonRunnerConfigs[index] as? JSObject {
                        let runnerConfig = try RunnerConfig(fromJSObject: jsonConfig)
                        self.runnerConfigs.append(runnerConfig)
                    }
                }
            }
        } catch {
            print(error.localizedDescription)
        }
    }
    
    @objc func dispatchEvent(_ call: CAPPluginCall) {
        call.unimplemented()
    }
    
    
    @objc private func didFinishLaunching() {
        self.registerRunnerTasks()
        print("did finish launching, register all tasks")
    }
    
    @objc private func didEnterBackground()  {
        self.runnerConfigs.forEach { config in
            self.scheduleRunnerTask(runnerConfig: config)
        }
        
        print("did enter background, schedule any work")
    }
    
    private func registerRunnerTasks() {
        self.runnerConfigs.forEach { runnerConfig in
            BGTaskScheduler.shared.register(forTaskWithIdentifier: runnerConfig.label, using: nil) { task in
                self.executeRunner(config: runnerConfig, task: task as! BGAppRefreshTask)
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
    
    private func executeRunner(config: RunnerConfig, task: BGAppRefreshTask) {
        print("successfully executing task")
        
        if config.repeats {
            self.scheduleRunnerTask(runnerConfig: config)
        }
        
        do {
            guard let srcFileURL = Bundle.main.url(forResource: config.src, withExtension: nil, subdirectory: "public") else {
                throw BackgroundRunnerPloginError.runnerError(reason: "source file not found")
            }
            
            let srcFile = try String(contentsOf: srcFileURL)
            
            let runner = Runner()
            let context = try runner.createContext(name: config.label)
            
            _ = try context.execute(code: srcFile)
            
            try context.dispatchEvent(event: config.event)
            
            task.setTaskCompleted(success: true)
        } catch {
            print(error)
            task.setTaskCompleted(success: false)
        }
    }
}
