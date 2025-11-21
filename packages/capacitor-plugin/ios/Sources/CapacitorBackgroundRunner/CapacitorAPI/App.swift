import Foundation
import UIKit
import JavaScriptCore

@objc protocol CapacitorAppExports: JSExport {
    static func getInfo() -> [String: Any]?
    static func getState() -> [String: Any]
}

class CapacitorApp: NSObject, CapacitorAppExports {
    static func getInfo() -> [String: Any]? {
        if let info = Bundle.main.infoDictionary {
            return [
                "name": info["CFBundleDisplayName"] as? String ?? "",
                "id": info["CFBundleIdentifier"] as? String ?? "",
                "build": info["CFBundleVersion"] as? String ?? "",
                "version": info["CFBundleShortVersionString"] as? String ?? ""
            ]
        }

        return nil
    }

    static func getState() -> [String: Any] {
        DispatchQueue.main.sync {
            return [
                "isActive": UIApplication.shared.applicationState == UIApplication.State.active
            ]
        }
    }
}
