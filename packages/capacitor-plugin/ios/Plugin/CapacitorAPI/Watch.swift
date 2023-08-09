import Foundation
import JavaScriptCore
import WatchConnectivity

@objc protocol CapacitorWatchExports: JSExport {
    func sendMessage(_ dataDict: JSValue) -> JSValue
    func transferUserInfo(_ userInfo: JSValue) -> JSValue
    func isReachable() -> JSValue
}

class CapacitorWatch: NSObject, CapacitorWatchExports {
    func sendMessage(_ dataDict: JSValue) -> JSValue {
        var dict: [String: Any] = [:]

        if dataDict.isObject {
            if let convertedDict = dataDict.toDictionary() as? [String: Any] {
                dict = convertedDict
            }
        }

        WCSession.default.sendMessage(dict, replyHandler: nil)

        return JSValue(undefinedIn: JSContext.current())
    }

    func transferUserInfo(_ userInfo: JSValue) -> JSValue {
        var dict: [String: Any] = [:]

        if userInfo.isObject {
            if let convertedDict = userInfo.toDictionary() as? [String: Any] {
                dict = convertedDict
            }
        }

        _ = WCSession.default.transferUserInfo(dict)

        return JSValue(undefinedIn: JSContext.current())
    }

    func updateApplicationContext(_ appContext: JSValue) -> JSValue {
        var dict: [String: Any] = [:]

        if userInfo.isObject {
            if let convertedDict = appContext.toDictionary() as? [String: Any] {
                dict = convertedDict
            }
        }

        _ = WCSession.default.updateApplicationContext(dict)

        return JSValue(undefinedIn: JSContext.current())
    }

    func isReachable() -> JSValue {
        if WCSession.default.isReachable {
            return JSValue(bool: true, in: JSContext.current())
        }

        return JSValue(bool: false, in: JSContext.current())
    }

    

}
