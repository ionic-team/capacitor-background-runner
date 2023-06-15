import Foundation
import JavaScriptCore
import UserNotifications

enum CapacitorNotificationsErrors: Error, Equatable {
    case invalidOptions(reason: String)
    case unknownError(reason: String)
    case permissionDenied
}

struct NotificationOptions {
    let title: String
    let body: String
    let threadIdentifier: String?
    
    init(from jsValue: JSValue) throws {
        guard let dict = jsValue.toDictionary() as? [String: Any?] else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "options are null")
        }
        
        guard let title = dict["title"] as? String else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification title is required")
        }
        
        guard let body = dict["body"] as? String else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification body is required")
        }
        
        self.title = title
        self.body = body
        self.threadIdentifier = dict["threadIdentifier"] as? String
    }
}

@objc protocol CapacitorNotificationsExports: JSExport {
    static func schedule(_ options: JSValue) -> Void
}

class CapacitorNotifications: NSObject, CapacitorNotificationsExports {
    static func checkPermission() -> String {
        let group = DispatchGroup()
        var permission: String = "prompt"
        
        group.enter()
        UNUserNotificationCenter.current().getNotificationSettings { settings in
            switch settings.authorizationStatus {
            case .authorized, .ephemeral, .provisional:
                permission = "granted"
            case .denied:
                permission = "denied"
            case .notDetermined:
                permission = "prompt"
            @unknown default:
                permission = "prompt"
            }
            group.leave()
        }
        
        group.wait()
        
        return permission
    }
    
    static func requestPermission() throws {
        var permissionsError: CapacitorNotificationsErrors?
        
        let group = DispatchGroup()
        
        group.enter()
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { granted, error in
            if let error = error {
                permissionsError = CapacitorNotificationsErrors.unknownError(reason:"\(error)")
            }
           
            group.leave()
        }
        
        group.wait()
        
        if let err = permissionsError {
            throw err
        }
    }
    
    
    static func schedule(_ options: JSValue) {
        do {
            if options.isUndefined || options.isNull {
                throw CapacitorNotificationsErrors.invalidOptions(reason: "options are null")
            }
            
            let options = try NotificationOptions(from: options)
            
            var permissionsError: CapacitorNotificationsErrors?
            var permissionGranted: Bool?
            
            
            let group = DispatchGroup()
            
            group.enter()
            
            // TODO: Handle permissions
            UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { granted, error in
                if let error = error {
                    permissionsError = CapacitorNotificationsErrors.unknownError(reason:"\(error)")
                    group.leave()
                    
                    return
                }
                
                permissionGranted = granted
                group.leave()
            }
            
            group.wait()
            
            if let err = permissionsError {
                throw err
            }
            
            if let granted = permissionGranted, granted {
                let notificationContent = UNMutableNotificationContent()
                notificationContent.title = options.title
                notificationContent.body = options.body
                
                if let id =  options.threadIdentifier {
                    notificationContent.threadIdentifier = id
                }
                
                let trigger = UNTimeIntervalNotificationTrigger(timeInterval: 3, repeats: false)
                let request = UNNotificationRequest(identifier: UUID().uuidString, content: notificationContent, trigger: trigger)
                
                UNUserNotificationCenter.current().add(request)
            } else {
                throw CapacitorNotificationsErrors.permissionDenied
            }
        } catch {
            let ex = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
            JSContext.current().exception = ex
        }
    }
}
