import Foundation
import Capacitor
import CapacitorJSRuntime

@objc(CapacitorNativeWorkerPlugin)
public class CapacitorNativeWorkerPlugin: CAPPlugin {
    private var runner: Runner?
    
    public override func load() {
        self.runner = Runner()
        
        if let runner = runner {
            do {
                _ = try runner.createContext(name: "io.ionic.starter")
                _ = runner.execute(name: "io.ionic.starter", sourcePath: "native-service-worker")
            } catch {
                print("NativeWorkerError: \(error.localizedDescription)")
            }
        }
        
        // 1.  Create a runner
        // 2.  Load the source code
        // 3.  Begin listening for events
    }
    
    @objc func dispatchEvent(_ call: CAPPluginCall) {
        guard let runner = runner else {
            call.reject("runner is nil")
            return
        }
        
        guard let event = call.getString("event") else {
            call.reject("event name is required to dispatch")
            return
        }
        
        runner.dispatchEvent(event: event)
        
    }
}
