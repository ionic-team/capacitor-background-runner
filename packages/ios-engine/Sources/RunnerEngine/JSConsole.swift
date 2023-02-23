import Foundation
import JavaScriptCore

@objc protocol JSConsoleExports: JSExport {
    func log (_ msg: String)
    func warn(_ msg: String)
    func error(_ msg: String)
    func debug(_ msg: String)
}

class JSConsole: NSObject, JSConsoleExports {
    let contextName: String
    
    init(name: String) {
        self.contextName = name
    }
    
    func log(_ msg: String) {
        print("[\(self.contextName)] [LOG] \(msg)")
    }
    
    func warn(_ msg: String) {
        print("[\(self.contextName)] [WARN] \(msg)")
    }
    
    func error(_ msg: String) {
        print("[\(self.contextName)] [ERR] \(msg)")
    }
    
    func debug(_ msg: String) {
        debugPrint("[\(self.contextName)] [DEBUG] \(msg)")
    }
}

