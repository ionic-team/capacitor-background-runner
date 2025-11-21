import Foundation
import UIKit
import JavaScriptCore

@objc protocol CapacitorDeviceExports: JSExport {
    func getNetworkStatus() -> [String: Any]?
    func getBatteryStatus() -> [String: Any]
}

enum CapacitorDeviceErrors: Error, Equatable {
    case unknownError(reason: String)
}

class CapacitorDevice: NSObject, CapacitorDeviceExports {
    var reachability: Reachability?
    var initError: Error?

    enum Connection {
        case unavailable, wifi, cellular
    }

    override init() {
        super.init()

        do {
            reachability = try Reachability()
        } catch {
            initError = error
        }
    }

    private func checkError() throws {
        if let err = initError {
            throw CapacitorDeviceErrors.unknownError(reason: "\(err)")
        }

        if reachability == nil {
            throw CapacitorDeviceErrors.unknownError(reason: "reachability failed to initialize")
        }
    }

    func getNetworkStatus() -> [String: Any]? {
        do {
            try checkError()

            let status = reachability?.connection.equivalentEnum ?? Connection.unavailable

            var networkStatusDict: [String: Any] = [:]
            networkStatusDict["connected"] = status.isConnected
            networkStatusDict["connectionType"] = status.jsStringValue

            return networkStatusDict
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            return nil
        }
    }

    func getBatteryStatus() -> [String: Any] {
        UIDevice.current.isBatteryMonitoringEnabled = true

        var batteryInfoDict: [String: Any] = [:]
        batteryInfoDict["batteryLevel"] = UIDevice.current.batteryLevel
        batteryInfoDict["isCharging"] = UIDevice.current.batteryState == .charging || UIDevice.current.batteryState == .full

        UIDevice.current.isBatteryMonitoringEnabled = false

        return batteryInfoDict
    }
}

fileprivate extension Reachability.Connection {
    var equivalentEnum: CapacitorDevice.Connection {
        switch self {
        case .unavailable:
            return .unavailable
        case .wifi:
            return .wifi
        case .cellular:
            return .cellular
        }
    }
}

extension CapacitorDevice.Connection {
    internal var jsStringValue: String {
        switch self {
        case .cellular:
            return "cellular"
        case .wifi:
            return "wifi"
        case .unavailable:
            return "none"
        }
    }

    internal var isConnected: Bool {
        switch self {
        case .cellular, .wifi:
            return true
        case .unavailable:
            return false
        }
    }

    internal var logMessage: String {
        switch self {
        case .cellular:
            return "Reachable via Cellular"
        case .wifi:
            return "Reachable via WiFi"
        case .unavailable:
            return "Not reachable"
        }
    }
}
