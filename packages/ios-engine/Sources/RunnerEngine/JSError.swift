import Foundation
import JavaScriptCore

// This is needed to do a problem with JavaScriptCore
// JSValue(newErrorFromMessage:in:) always returns a nil object
//
// https://github.com/mihaibabusca/jscore-behaviour-mismatch

@objc protocol JSErrorExports: JSExport {
    var cause: Any? { get set }
    var message: String { get set }

    func toString() -> String
}

@objc public class JSError: NSObject, JSErrorExports {
    dynamic var cause: Any?
    dynamic var message: String

    override public init() {
        message = ""
        cause = nil
    }

    public init(message: String) {
        self.message = message
        self.cause = nil
    }

    func toString() -> String {
        return message
    }
}
