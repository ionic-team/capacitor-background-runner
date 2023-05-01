import Foundation
import JavaScriptCore
import WatchConnectivity

@objc protocol CapacitorWearableExports: JSExport {
    func send(_ dataDict: JSValue) ->JSValue
}

// TODO: JEDI Prototype Experiment, replace with better fleshed out,
// cross-platform supported API
class CapacitorWearable: NSObject, CapacitorWearableExports {
    private weak var context: JSContext?
    
    init(context: JSContext) {
        self.context = context
    }
    
    func send(_ dataDict: JSValue) -> JSValue {
        var dict: [String: Any] = [:]
        
        if dataDict.isObject {
            if let convertedDict = dataDict.toDictionary() as? [String: Any] {
                dict = convertedDict
            }
        }
        
        let _ = WCSession.default.transferUserInfo(dict)
        
        return JSValue(undefinedIn: self.context)
    }
    
    
}


