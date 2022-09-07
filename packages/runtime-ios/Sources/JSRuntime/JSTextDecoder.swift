import JavaScriptCore

@objc protocol JSTextDecoderExports: JSExport {
    var encoding: String { get }
    func decode(_ buffer: JSValue) -> String?
}

class JSTextDecoder: NSObject, JSTextDecoderExports {
    dynamic var encoding: String = "utf-8"
    
    required init(encoding: String?, options: [AnyHashable: Any]?) {
        if let enc = encoding {
            self.encoding = enc
        }
    }
    
    func decode(_ buffer: JSValue) -> String? {
        let useEncoding = setEncoding(enc: self.encoding)
        
        if let arr = buffer.toArray() as? [UInt8] {
            return String(bytes: arr, encoding: useEncoding)
        }
        
        if let arr = buffer.toArray() as? [Int8] {
            let data = Data(bytes: arr, count: arr.count)
            return String(data: data, encoding: useEncoding)
        }
        
        return nil
    }
    
    func setEncoding(enc: String) -> String.Encoding {
        if (enc == "windows-1250") {
            return .windowsCP1250
        }
        
        if (enc == "windows-1251") {
            return .windowsCP1251
        }
        
        if (enc == "windows-1252") {
            return .windowsCP1252
        }
        
        if (enc == "windows-1253") {
            return .windowsCP1253
        }
        
        if (enc == "windows-1254") {
            return .windowsCP1254
        }
        
        if (enc == "iso-2022-jp") {
            return .iso2022JP
        }
        
        if (enc == "euc-jp") {
            return .japaneseEUC
        }
        
        if (enc == "macintosh") {
            return .macOSRoman
        }
        
        if (enc == "iso-8859-2") {
            return .isoLatin2
        }
        
        if (enc == "utf-16be") {
            return .utf16BigEndian
        }
        
        if (enc == "utf-16le") {
            return .utf16LittleEndian
        }
        
        return .utf8
    }
}
