import Foundation
import JavaScriptCore
import CoreLocation

@objc protocol CapacitorGeolocationExports: JSExport {
    func getCurrentPosition() -> [String: Double]?
}

enum CapacitorGeolocationErrors: Error, Equatable {
    case unauthorized
}


class CapacitorGeolocation: NSObject, CapacitorGeolocationExports, CLLocationManagerDelegate {
    private let locationManager: CLLocationManager
    
    override init() {
        self.locationManager = CLLocationManager()
        super.init()
        
        self.locationManager.delegate = self
    }
    
    func checkPermission() throws {
        if #available(iOS 14.0, *) {
            switch self.locationManager.authorizationStatus {
            case .authorizedWhenInUse:
                return
            default:
                throw CapacitorGeolocationErrors.unauthorized
            }
        } else {
            // Fallback on earlier versions
        }
    }
    
    func getCurrentPosition() -> [String: Double]? {
        do {
            try self.checkPermission()
            return [:]
        } catch {
            let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            JSContext.current().exception = ex
            
            return nil
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        if let location = locations.first {
            print("\(location.coordinate.latitude),\(location.coordinate.longitude)")
        }
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("\(error)")
    }
}

