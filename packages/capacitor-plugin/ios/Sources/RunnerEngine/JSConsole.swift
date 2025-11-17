import Foundation
import JavaScriptCore

@objc protocol JSConsoleExports: JSExport {
    func log (_ msg: String)
    func info(_ msg: String)
    func warn(_ msg: String)
    func error(_ msg: String)
    func debug(_ msg: String)
}

class JSConsole: NSObject, JSConsoleExports {
    let contextName: String

    init(name: String) {
        contextName = name
    }

    func log(_ msg: String) {
        print("[\(contextName)] [LOG] \(msg)")
    }

    func info(_ msg: String) {
        print("[\(contextName)] [INFO] \(msg)")
    }

    func warn(_ msg: String) {
        print("[\(contextName)] [WARN] \(msg)")
    }

    func error(_ msg: String) {
        print("[\(contextName)] [ERR] \(msg)")
    }

    func debug(_ msg: String) {
        debugPrint("[\(contextName)] [DEBUG] \(msg)")
    }
}
