import Foundation
import JavaScriptCore
import CoreLocation

@objc protocol CapacitorGeolocationExports: JSExport {
    var isWatchingPosition: Bool { get }
    func startWatchingPosition(_ highAccuracy: Bool) -> Void
    func stopWatchingPosition() -> Void
    func getCurrentPosition() -> [String: Any]?
}

enum CapacitorGeolocationErrors: Error, Equatable {
    case unauthorized
}

class CapacitorGeolocation: NSObject, CapacitorGeolocationExports, CLLocationManagerDelegate {
    var isWatchingPosition: Bool {
        return isWatchingLocation
    }
    
    private weak var context: Context?
    private var pendingCurrentLocationCalls: [Int: (Result<CLLocation?, Error>) -> Void] = [:]
    private var isWatchingLocation: Bool = false
    private let locationManager: CLLocationManager
    
    init(context: Context) {
        self.locationManager = CLLocationManager()
        self.context = context
        
        super.init()
        
        self.locationManager.allowsBackgroundLocationUpdates = true
        self.locationManager.desiredAccuracy = kCLLocationAccuracyKilometer
        self.locationManager.pausesLocationUpdatesAutomatically = false
        self.locationManager.delegate = self
        
        self.locationManager.startMonitoringSignificantLocationChanges()
        self.locationManager.stopMonitoringSignificantLocationChanges()
    }
    
    static func getCurrentKnownLocation() -> CLLocation? {
        return CLLocationManager().location
    }
    
    func checkPermission() throws {
        if #available(iOS 14.0, *) {
            switch self.locationManager.authorizationStatus {
            case .authorizedWhenInUse:
                fallthrough
            case .authorizedAlways:
                fallthrough
            case .authorized:
                return
            case .notDetermined:
                self.locationManager.requestAlwaysAuthorization()
                break
            default:
                throw CapacitorGeolocationErrors.unauthorized
            }
        } else {
            // Fallback on earlier versions
        }
    }
    
    func startWatchingPosition(_ highAccuracy: Bool) {
        do {
            try self.checkPermission()
            
            DispatchQueue.main.sync {
                if highAccuracy {
                    self.locationManager.startUpdatingLocation()
                } else {
                    self.locationManager.startMonitoringSignificantLocationChanges()
                }
            }
            
            self.isWatchingLocation = true
        } catch {
            let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            JSContext.current().exception = ex
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
            try self.checkPermission()
            
            guard let lastLocation = self.locationManager.location else {
                return nil
            }
            
            return buildLocationDict(location: lastLocation)
        } catch {
            let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            JSContext.current().exception = ex
            
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
        if let context = self.context, self.isWatchingLocation {
            var details: [String: Any] = [:]
            details["error"] = error
            
            try? context.dispatchEvent(event: "currentLocation", details: details)
        }
        
        pendingCurrentLocationCalls.forEach { (callId: Int, callback: (Result<CLLocation?, Error>) -> Void) in
            pendingCurrentLocationCalls.removeValue(forKey: callId)
            callback(.failure(error))
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

