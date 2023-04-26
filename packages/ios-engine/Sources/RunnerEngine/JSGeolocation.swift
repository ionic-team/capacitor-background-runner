import Foundation
import JavaScriptCore
import CoreLocation

@objc protocol JSGeolocationExports: JSExport {
    func getCurrentPosition() -> JSValue
}

enum JSGeolocationErrors: Error, Equatable {
    case unauthorized
}


class JSGeolocation: NSObject, JSGeolocationExports, CLLocationManagerDelegate {
    private let context: JSContext
    private let locationManager: CLLocationManager
    
    init(context: JSContext) {
        self.context = context
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
                throw JSGeolocationErrors.unauthorized
            }
        } else {
            // Fallback on earlier versions
        }
    }
    
    func getCurrentPosition() -> JSValue {
        return JSValue(newPromiseIn: self.context) { resolve, reject in
            do {
                try self.checkPermission()
                
                self.locationManager.requestLocation()
                
                resolve?.call(withArguments: [])
            } catch {
                let jsError = JSValue(newErrorFromMessage: error.localizedDescription, in: self.context)
        
                reject?.call(withArguments: [jsError])
            }
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
