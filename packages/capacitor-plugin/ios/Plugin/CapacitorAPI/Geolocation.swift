import Foundation
import JavaScriptCore
import CoreLocation

@objc protocol CapacitorGeolocationExports: JSExport {
    func startWatchingPosition() -> Void
    func stopWatchingPosition() -> Void
    func getCurrentPosition() -> JSValue
    func getLastPosition() -> [String: Double]?
}

enum CapacitorGeolocationErrors: Error, Equatable {
    case unauthorized
}

class CapacitorGeolocation: NSObject, CapacitorGeolocationExports, CLLocationManagerDelegate {
    private weak var context: Context?
    private var pendingCurrentLocationCalls: [Int: (Result<CLLocation?, Error>) -> Void] = [:]
    private var isWatchingLocation: Bool = false
    private let locationManager: CLLocationManager
    
    init(context: Context) {
        self.locationManager = CLLocationManager()
        self.context = context
        
        super.init()
        
        self.locationManager.allowsBackgroundLocationUpdates = true
        self.locationManager.desiredAccuracy = kCLLocationAccuracyThreeKilometers
        self.locationManager.delegate = self
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
    
    func startWatchingPosition()  {
        do {
            try self.checkPermission()
            
            DispatchQueue.main.sync {
                self.locationManager.startUpdatingLocation()
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
        }
    
        self.isWatchingLocation = false
    }
    
    func getCurrentPosition() -> JSValue {
        return JSValue(newPromiseIn: JSContext.current()) { resolve, reject in
            do {
                try self.checkPermission()
                
                let group = DispatchGroup()
                
                var location: CLLocation?
                var err: Error?
                
                let callId = self.pendingCurrentLocationCalls.count + 1
                
                let callback: (Result<CLLocation?, Error>) -> Void = { result in
                    switch result {
                    case .success(let resolvedLocation):
                        location = resolvedLocation
                    case .failure(let resolvedErr):
                        err = resolvedErr
                    }
                    
                    group.leave()
                }
                
                self.pendingCurrentLocationCalls[callId] = callback
                
                group.enter()
                DispatchQueue.main.sync {
                    self.locationManager.requestLocation()
                }
                
                group.wait()
                
                if let err = err {
                    let ex = JSValue(newErrorFromMessage: "\(err)", in: JSContext.current())
                    reject?.call(withArguments: [ex])
                    return
                }
                
                guard let location = location else {
                    resolve?.call(withArguments: [])
                    return
                }
                
                var coordDict: [String: Double] = [:]
                coordDict["lat"] = location.coordinate.latitude
                coordDict["lng"] = location.coordinate.longitude
                coordDict["altitude"] = location.altitude
                
                resolve?.call(withArguments: [coordDict])
            } catch {
                let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
                reject?.call(withArguments: [ex])
            }
        }
    }
    
    func getLastPosition() -> [String : Double]? {
        do {
            try self.checkPermission()
            
            guard let lastLocation = self.locationManager.location else {
                return nil
            }
            
            var coordDict: [String: Double] = [:]
            coordDict["lat"] = lastLocation.coordinate.latitude
            coordDict["lng"] = lastLocation.coordinate.longitude
            coordDict["altitude"] = lastLocation.altitude
            
            if let heading = self.locationManager.heading {
                coordDict["heading"] = heading.magneticHeading
            }
            
            return coordDict
        } catch {
            let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            JSContext.current().exception = ex
            
            return nil
        }
    }
        
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        if let context = self.context, self.isWatchingLocation {
            let locations: [[String: Double]] = locations.map { location in
                var coordDict: [String: Double] = [:]
                coordDict["lat"] = location.coordinate.latitude
                coordDict["lng"] = location.coordinate.longitude
                coordDict["altitude"] = location.altitude
                
                return coordDict
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
}

