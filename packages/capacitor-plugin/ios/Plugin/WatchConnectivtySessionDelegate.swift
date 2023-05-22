import Foundation
import WatchConnectivity

extension BackgroundRunnerPlugin: WCSessionDelegate {
    public func session(_ session: WCSession, activationDidCompleteWith activationState: WCSessionActivationState, error: Error?) {
        var args: [String: Any] = [:]
        args["activationState"] = activationState.rawValue
        args["error"] = error
        
        BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_activationDidCompleteWith", inputArgs: args)
    }
    
    public func sessionDidBecomeInactive(_ session: WCSession) {
        BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_sessionDidBecomeInactive")
    }
    
    public func sessionDidDeactivate(_ session: WCSession) {
        BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_sessionDidDeactivate")
    }
    
    public func session(_ session: WCSession, didReceiveUserInfo userInfo: [String : Any] = [:]) {
        var args: [String: Any] = [:]
        args["userInfo"] = userInfo
        
        BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_didReceiveUserInfo", inputArgs: args)
    }
    
    public func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        var args: [String: Any] = [:]
        args["message"] = message
        
        BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_didReceiveMessage", inputArgs: args)
    }
}
