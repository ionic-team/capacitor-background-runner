import Foundation
import JavaScriptCore

@objc protocol JSConsoleExports: JSExport {
    static func log (_ msg: String)
    static func warn(_ msg: String)
    static func error(_ msg: String)
}

class JSConsole: NSObject, JSConsoleExports {
    static func log(_ msg: String) {
        print("[INFO] \(msg)")
    }
    
    static func warn(_ msg: String) {
        print("[WARN] \(msg)")
    }
    
    static func error(_ msg: String) {
        print("[ERR] \(msg)")
    }
}
