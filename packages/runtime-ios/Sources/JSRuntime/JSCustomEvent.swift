import Foundation
import JavaScriptCore

@objc protocol JSCustomEventExports: JSExport {
    var type: String { get set }
    var detail: [AnyHashable: Any] { get set }
    
    static func initWithType(type: String, options: [AnyHashable: Any]?) -> JSCustomEvent
}

@objc public class JSCustomEvent: NSObject, JSCustomEventExports {
    dynamic var type: String
    dynamic var detail: [AnyHashable: Any]
    
    required init(type: String, options: [AnyHashable: Any]?) {
        self.type = type
        self.detail = [AnyHashable: Any]()
        
        if let options = options {
            if let detail = options["detail"] as? [AnyHashable: Any] {
                self.detail = detail
            }
        }
    }
    
    class func initWithType(type: String, options: [AnyHashable: Any]?) -> JSCustomEvent {
        return JSCustomEvent(type: type, options: options)
    }
}
