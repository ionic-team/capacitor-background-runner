import Foundation
import JavaScriptCore
import WatchConnectivity

@objc protocol CapacitorWearableExports: JSExport {
    func send(_ dataDict: JSValue) ->JSValue
    func transferUserInfo(_ userInfo: JSValue) -> JSValue
    func isReachable() -> JSValue
}

// TODO: JEDI Prototype Experiment, replace with better fleshed out,
// cross-platform supported API
class CapacitorWearable: NSObject, CapacitorWearableExports {
    func send(_ dataDict: JSValue) -> JSValue {
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
        
        let _ = WCSession.default.transferUserInfo(dict)
        
        return JSValue(undefinedIn: JSContext.current())
    }
    
    func isReachable() -> JSValue {
        if WCSession.default.isReachable {
            return JSValue(bool: true, in: JSContext.current())
        }
        
        return JSValue(bool: false, in: JSContext.current())
    }
    
    
}


