import JavaScriptCore

@objc protocol JSTextEncoderExports: JSExport {
    func encode(_ str: String) -> [UInt8]
    //    func encodeInto(_ str: String, _ arr: JSValue) -> [AnyHashable: Any]
}

class JSTextEncoder: NSObject, JSTextEncoderExports {
    func encode(_ str: String) -> [UInt8] {
        return Array(str.utf8)
    }
}
