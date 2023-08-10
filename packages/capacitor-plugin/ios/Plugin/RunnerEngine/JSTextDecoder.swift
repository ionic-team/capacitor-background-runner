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
        let useEncoding = setEncoding(enc: encoding)

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
        switch enc {
        case "windows-1250":
            return .windowsCP1250
        case "windows-1251":
            return .windowsCP1251
        case "windows-1252":
            return .windowsCP1252
        case "windows-1253":
            return .windowsCP1253
        case "windows-1254":
            return .windowsCP1254
        case "iso-2022-jp":
            return .iso2022JP
        case "euc-jp":
            return .japaneseEUC
        case "macintosh":
            return .macOSRoman
        case "iso-8859-2":
            return .isoLatin2
        case "utf-16be":
            return .utf16BigEndian
        case "utf-16le":
            return .utf16LittleEndian
        default:
            return .utf8
        }
    }
}
