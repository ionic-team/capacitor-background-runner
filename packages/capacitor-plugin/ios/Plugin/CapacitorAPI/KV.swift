import Foundation
import JavaScriptCore

@objc protocol CapacitorKVStoreExports: JSExport {
    static func set(_ key: String, _ value: String)
    static func get(_ key: String) -> JSValue
    static func remove(_ key: String)
}

class CapacitorKVStore: NSObject, CapacitorKVStoreExports {
    static func set(_ key: String, _ value: String) {
        UserDefaults.standard.set(value, forKey: key)
    }

    static func get(_ key: String) -> JSValue {
        guard let value = UserDefaults.standard.string(forKey: key) else {
            return JSValue(nullIn: JSContext.current())
        }

        var valueWrapper: [String: String] = [:]
        valueWrapper["value"] = value

        return JSValue(object: valueWrapper, in: JSContext.current())
    }

    static func remove(_ key: String) {
        return UserDefaults.standard.removeObject(forKey: key)
    }
}
