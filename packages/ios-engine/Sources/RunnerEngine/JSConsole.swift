import Foundation
import JavaScriptCore
import OSLog

@objc protocol JSConsoleExports: JSExport {
    func log (_ msg: String)
    func info(_ msg: String)
    func warn(_ msg: String)
    func error(_ msg: String)
    func debug(_ msg: String)
}

class JSConsole: NSObject, JSConsoleExports {
    let contextName: String
    let contextLogger: Logger

    init(name: String, logger: Logger) {
        contextName = name
        contextLogger = logger
    }

    func log(_ msg: String) {
        contextLogger.log("\(msg)")
    }

    func info(_ msg: String) {
        contextLogger.info("\(msg)")
    }

    func warn(_ msg: String) {
        contextLogger.warning("\(msg)")
    }

    func error(_ msg: String) {
        contextLogger.error("\(msg)")
    }

    func debug(_ msg: String) {
        contextLogger.debug("\(msg)")
    }
}
