import Foundation
import JavaScriptCore

@objc protocol JSKVExports: JSExport {
    static func set(_ key: String, _ value: String)
    static func get(_ key: String) -> String?
    static func remove(_ key: String)
}

class JSKV: NSObject, JSKVExports {
    static func set(_ key: String, _ value: String) {
        UserDefaults.standard.set(value, forKey: key)
    }

    static func get(_ key: String) -> String? {
        return UserDefaults.standard.string(forKey: key)
    }
    
    static func remove(_ key: String) {
        return UserDefaults.standard.removeObject(forKey: key)
    }
}
