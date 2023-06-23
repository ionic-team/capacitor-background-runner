import Foundation
import JavaScriptCore
import CoreLocation

@objc protocol CapacitorGeolocationExports: JSExport {
    var isWatchingPosition: Bool { get }
    func startWatchingPosition(_ highAccuracy: Bool)
    func stopWatchingPosition()
    func getCurrentPosition() -> [String: Any]?
}

enum CapacitorGeolocationErrors: Error, Equatable {
    case unauthorized
}

class CapacitorGeolocation: NSObject, CapacitorGeolocationExports, CLLocationManagerDelegate {
    private weak var context: Context?
    private var pendingCurrentLocationCalls: [Int: (Result<CLLocation?, Error>) -> Void] = [:]
    private var isWatchingLocation: Bool = false
    private let locationManager: CLLocationManager

    private var permissionContinuation: CheckedContinuation<Void, Error>?

    var isWatchingPosition: Bool {
        return isWatchingLocation
    }

    init(context: Context? = nil) {
        self.locationManager = CLLocationManager()
        super.init()

        self.context = context
        self.locationManager.delegate = self

        self.locationManager.allowsBackgroundLocationUpdates = true
        self.locationManager.desiredAccuracy = kCLLocationAccuracyKilometer
        self.locationManager.pausesLocationUpdatesAutomatically = false
    }

    static func getCurrentKnownLocation() -> CLLocation? {
        return CLLocationManager().location
    }

    static func checkPermission() -> String {
        if !CLLocationManager.locationServicesEnabled() {
            return "denied"
        }

        var permission: String = "prompt"

        switch CLLocationManager.authorizationStatus() {
        case .authorized, .authorizedWhenInUse, .authorizedAlways:
            permission = "granted"
        case .notDetermined:
            permission = "prompt"
        case .denied, .restricted:
            permission = "denied"
        default:
            permission = "prompt"
        }

        return permission
    }

    func requestPermission() async throws {
        if !CLLocationManager.locationServicesEnabled() {
            return
        }

        if CLLocationManager.authorizationStatus() != .notDetermined {
            return
        }

        return try await withCheckedThrowingContinuation({ (continuation: CheckedContinuation<Void, Error>) in
            permissionContinuation = continuation

            DispatchQueue.main.async {
                self.locationManager.requestAlwaysAuthorization()
            }
        })
    }

    func startWatchingPosition(_ highAccuracy: Bool) {
        do {
            if CapacitorGeolocation.checkPermission() != "granted" {
                throw CapacitorGeolocationErrors.unauthorized
            }

            DispatchQueue.main.sync {
                if highAccuracy {
                    self.locationManager.startUpdatingLocation()
                } else {
                    self.locationManager.startMonitoringSignificantLocationChanges()
                }
            }

            self.isWatchingLocation = true
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
        }
    }

    func stopWatchingPosition() {
        DispatchQueue.main.sync {
            self.locationManager.stopUpdatingLocation()
            self.locationManager.stopMonitoringSignificantLocationChanges()
        }

        self.isWatchingLocation = false
    }

    func getCurrentPosition() -> [String: Any]? {
        do {
            if CapacitorGeolocation.checkPermission() != "granted" {
                throw CapacitorGeolocationErrors.unauthorized
            }

            guard let lastLocation = self.locationManager.location else {
                return nil
            }

            return buildLocationDict(location: lastLocation)
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())

            return nil
        }
    }

    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        if let context = self.context, self.isWatchingLocation {
            let locations: [[String: Any]] = locations.map { location in
                return buildLocationDict(location: location)
            }

            var details: [String: Any] = [:]
            details["locations"] = locations

            try? context.dispatchEvent(event: "currentLocation", details: details)
        }

        pendingCurrentLocationCalls.forEach { (callId: Int, callback: (Result<CLLocation?, Error>) -> Void) in
            pendingCurrentLocationCalls.removeValue(forKey: callId)
            callback(.success(locations.last))
        }

        if let location = locations.last {
            print("\(location.coordinate.latitude),\(location.coordinate.longitude)")
        }
    }

    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        if let continuation = permissionContinuation {
            continuation.resume(throwing: error)
            permissionContinuation = nil
        }

        if let context = context, self.isWatchingLocation {
            var details: [String: Any] = [:]
            details["error"] = error

            try? context.dispatchEvent(event: "currentLocation", details: details)
        }

        pendingCurrentLocationCalls.forEach { (callId: Int, callback: (Result<CLLocation?, Error>) -> Void) in
            pendingCurrentLocationCalls.removeValue(forKey: callId)
            callback(.failure(error))
        }
    }

    func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        if let continuation = permissionContinuation {
            continuation.resume()
            permissionContinuation = nil
        }
    }

    private func buildLocationDict(location: CLLocation) -> [String: Any] {
        var coords: [String: Any] = [:]
        coords["latitude"] = location.coordinate.latitude
        coords["longitude"] = location.coordinate.longitude
        coords["accuracy"] = location.horizontalAccuracy
        coords["altitude"] = location.altitude
        coords["altitudeAccuracy"] = location.verticalAccuracy
        coords["speed"] = location.speed
        coords["heading"] = location.course

        return coords
    }
}
