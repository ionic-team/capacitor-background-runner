import Foundation
import JavaScriptCore
import WatchConnectivity

@objc protocol CapacitorWatchExports: JSExport {
    func sendMessage(_ dataDict: JSValue) -> JSValue
    func transferUserInfo(_ userInfo: JSValue) -> JSValue
    func updateApplicationContext(_ appContext: JSValue) -> JSValue
    func isReachable() -> JSValue
    func updateWatchUI(_ call: JSValue)
    func updateWatchData(_ call: JSValue)
}

// swiftlint:disable:next identifier_name
public let UI_KEY = "watchUI"
// swiftlint:disable:next identifier_name
public let DATA_KEY = "viewData"

class CapacitorWatch: NSObject, CapacitorWatchExports {
    func sendMessage(_ dataDict: JSValue) -> JSValue {
        let dict = jsValueToDict(dataDict)

        WCSession.default.sendMessage(dict, replyHandler: nil)

        return JSValue(undefinedIn: JSContext.current())
    }

    func transferUserInfo(_ userInfo: JSValue) -> JSValue {
        let dict = jsValueToDict(userInfo)

        WCSession.default.transferUserInfo(dict)

        return JSValue(undefinedIn: JSContext.current())
    }

    func updateApplicationContext(_ appContext: JSValue) -> JSValue {
        let dict = jsValueToDict(appContext)

        try? WCSession.default.updateApplicationContext(dict)

        return JSValue(undefinedIn: JSContext.current())
    }

    func isReachable() -> JSValue {
        if WCSession.default.isReachable {
            return JSValue(bool: true, in: JSContext.current())
        }

        return JSValue(bool: false, in: JSContext.current())
    }

    func updateWatchUI(_ call: JSValue) {
        let dict = jsValueToDict(call)

        guard let newUI = dict["data"] as? [String: String] else {
            return
        }

        WCSession.default.transferUserInfo([UI_KEY: newUI])
    }

    func updateWatchData(_ call: JSValue) {
        let dict = jsValueToDict(call)

        guard let newData = dict["data"] as? [String: String] else {
            return
        }

        WCSession.default.transferUserInfo([DATA_KEY: newData])
    }

    fileprivate func jsValueToDict(_ jsVal: JSValue) -> [String: Any] {
        var dict: [String: Any] = [:]

        if jsVal.isObject {
            if let convertedDict = jsVal.toDictionary() as? [String: Any] {
                dict = convertedDict
            }
        }

        return dict
    }
}

enum WatchDataError: Error {
    case JSValIsNotObject
    // swiftlint:disable:next identifier_name
    case CouldNotConvertJSVal
}
