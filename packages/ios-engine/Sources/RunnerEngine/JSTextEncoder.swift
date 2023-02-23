import JavaScriptCore

@objc protocol JSTextEncoderExports: JSExport {
    func encode(_ str: String) -> [UInt8]
//    func encodeInto(_ str: String, _ arr: JSValue) -> [AnyHashable: Any]
}

class JSTextEncoder: NSObject, JSTextEncoderExports {
    func encode(_ str: String) -> [UInt8] {
        return Array(str.utf8)
    }
    
//    func encodeInto(_ str: String, _ arr: JSValue) -> [AnyHashable: Any] {
//        var result = [String: Any]()
//
//        let strArr = Array(str.utf8)
//        result["read"] = strArr.count
//
//        for i in 0...(arr.toArray().capacity - 1) {
//            arr.setObject(UInt8(strArr[i]), atIndexedSubscript: i)
//        }
//
//        result["written"] = arr.toArray().capacity
//
//        return result
//    }
}
