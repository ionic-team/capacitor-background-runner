import Foundation
import JavaScriptCore

@objc protocol JSCustomEventExports: JSExport {
    var type: String { get set }
    
    static func initWithType(type: String) -> JSCustomEvent
}

@objc public class JSCustomEvent: NSObject, JSCustomEventExports {
    dynamic var type: String
    
    required init(type: String) {
        self.type = type
    }
    
    class func initWithType(type: String) -> JSCustomEvent {
        return JSCustomEvent(type: type)
    }
}
