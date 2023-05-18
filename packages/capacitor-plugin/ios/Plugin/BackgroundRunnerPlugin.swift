import Foundation
import Capacitor
import WatchConnectivity

/**
 * Please read the Capacitor iOS Plugin Development Guide
 * here: https://capacitorjs.com/docs/plugins/ios
 */
@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin {
    private let impl = BackgroundRunner()
    
    override public func load() {
        NotificationCenter.default.addObserver(self, selector: #selector(didEnterBackground), name: UIApplication.didEnterBackgroundNotification, object: nil)
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
            
            guard let config = impl.getConfig(name: runnerLabel) else {
                throw BackgroundRunnerPluginError.runnerError(reason: "no runner config found for label")
            }
            
            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    let result = try impl.execute(config: config, task: nil, inputArgs: details as [String: Any], eventOverride: runnerEvent)
                    
                    if let result = result {
                        call.resolve(result)
                        return
                    }
                    
                    call.resolve()
                } catch {
                    call.reject("\(error)")
                }
            }
        } catch {
            call.reject("\(error)")
        }
    }
    
    @objc func registerBackgroundTask(_ call: CAPPluginCall) {
        call.resolve()
    }
    
    @objc private func didEnterBackground()  {
        impl.scheduleBackgroundTasks()
    }
    
    public static func registerBackgroundTasks() {
        BackgroundRunner.shared.registerBackgroundTasks()
    }
    
    public static func handleApplicationDidFinishLaunching(launchOptions: [UIApplication.LaunchOptionsKey: Any]?) {
        guard let launchOptions = launchOptions else {
            return
        }
        
        var events: [String: [String: Any]?] = [:]
        
        if launchOptions[.location] != nil {
            // application was launched for a location event
            var details: [String: Any] = [:]
            details["locations"] = nil
            
            if let location = CapacitorGeolocation.getCurrentKnownLocation() {
                var details: [String: Any] = [:]
                details["locations"] = [location]
            }
            
            events["currentLocation"] = details
        }
        
        print("emit events: \(events)")
        
    }
    
    private func initWatchConnectivity() {
        if !WCSession.isSupported() {
            return
        }
        
        WCSession.default.delegate = self
        WCSession.default.activate()
    }
}
