import Foundation
import WatchConnectivity

extension BackgroundRunnerPlugin: WCSessionDelegate {
    public func session(_ session: WCSession, activationDidCompleteWith activationState: WCSessionActivationState, error: Error?) {
        var args: [String: Any] = [:]
        args["activationState"] = activationState.rawValue
        args["error"] = error
        
        self.globalDispatchEvent(event: "WatchConnectivity_activationDidCompleteWith", args: args)
    }
    
    public func sessionDidBecomeInactive(_ session: WCSession) {
        self.globalDispatchEvent(event: "WatchConnectivity_sessionDidBecomeInactive")
    }
    
    public func sessionDidDeactivate(_ session: WCSession) {
        self.globalDispatchEvent(event: "WatchConnectivity_sessionDidDeactivate")
    }
    
    public func session(_ session: WCSession, didReceiveUserInfo userInfo: [String : Any] = [:]) {
        var args: [String: Any] = [:]
        args["userInfo"] = userInfo
        
        self.globalDispatchEvent(event: "WatchConnectivity_didReceiveUserInfo", args: args)
    }
    
    public func session(_ session: WCSession, didReceiveMessage message: [String : Any]) {
        var args: [String: Any] = [:]
        args["message"] = message
        
        self.globalDispatchEvent(event: "WatchConnectivity_didReceiveMessage", args: args)
    }
}
