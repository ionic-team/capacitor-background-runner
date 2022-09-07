import Foundation
import JavaScriptCore

@objc protocol JSPromiseExports: JSExport {
    func then(_ resolve: JSValue) -> JSPromise?
    func `catch`(_ reject: JSValue) -> JSPromise?
}


class JSPromise: NSObject, JSPromiseExports {
    dynamic var resolveFunc: JSValue?
    dynamic var rejectFunc: JSValue?
    
    
    required init(callback: JSValue?) {
        self.resolveFunc = callback?.objectAtIndexedSubscript(0)
        self.rejectFunc = callback?.objectAtIndexedSubscript(1)
    }
    
    func then(_ resolve: JSValue) -> JSPromise? {
        return nil
    }
    
    func `catch`(_ reject: JSValue) -> JSPromise? {
        return nil
    }
    
    
}
