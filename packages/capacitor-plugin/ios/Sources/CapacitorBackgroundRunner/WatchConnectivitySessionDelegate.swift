import Foundation
import WatchConnectivity

extension BackgroundRunnerPlugin: WCSessionDelegate {
    public func session(_ session: WCSession, activationDidCompleteWith activationState: WCSessionActivationState, error: Error?) {
        var args: [String: Any] = [:]
        args["activationState"] = activationState.rawValue
        args["error"] = error

        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_activationDidCompleteWith", inputArgs: args)
    }

    public func sessionDidBecomeInactive(_ session: WCSession) {
        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_sessionDidBecomeInactive", inputArgs: nil)
    }

    public func sessionDidDeactivate(_ session: WCSession) {
        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_sessionDidDeactivate", inputArgs: nil)
    }

    // DCG Note - documentation for these events are contained within the CapacitorWatch documentation

    public func session(_ session: WCSession, didReceiveUserInfo userInfo: [String: Any] = [:]) {
        var args: [String: Any] = [:]
        args["userInfo"] = userInfo

        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_didReceiveUserInfo", inputArgs: args)
    }

    public func session(_ session: WCSession, didReceiveMessage message: [String: Any]) {
        var args: [String: Any] = [:]
        args["message"] = message

        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_didReceiveMessage", inputArgs: args)
    }

    public func session(_ session: WCSession, didReceiveApplicationContext applicationContext: [String: Any]) {
        var args: [String: Any] = [:]
        args["message"] = applicationContext

        try? BackgroundRunner.shared.dispatchEvent(event: "WatchConnectivity_didReceiveApplicationContext", inputArgs: args)
    }
}
