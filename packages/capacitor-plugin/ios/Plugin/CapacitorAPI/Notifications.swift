import Foundation
import JavaScriptCore
import UserNotifications

enum CapacitorNotificationsErrors: Error, Equatable {
    case invalidOptions(reason: String)
    case unknownError(reason: String)
    case permissionDenied
}

struct SetBadgeOption {
    let count: Int

    init(from dict: [String: Any?]) {
        if let optionsCount = dict["count"] as? Int {
            count = optionsCount
        } else {
            count = 0
        }
    }
}

struct NotificationOption {
    let id: Int
    let title: String
    let body: String
    let scheduleAt: Date
    let sound: String?
    let actionTypeId: String?
    let extra: [String: Any]?
    let threadIdentifier: String?
    let summaryArgument: String?
    let groupSummary: String?

    init(from dict: [String: Any?]) throws {
        guard let id = dict["id"] as? Int else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification id is required")
        }

        guard let title = dict["title"] as? String else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification title is required")
        }

        guard let body = dict["body"] as? String else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification body is required")
        }

        guard let scheduleAt = dict["scheduleAt"] as? Date else {
            throw CapacitorNotificationsErrors.invalidOptions(reason: "notification schedule date is required")
        }

        self.id = id
        self.title = title
        self.body = body
        self.scheduleAt = scheduleAt
        self.threadIdentifier = dict["threadIdentifier"] as? String
        self.sound = dict["sound"] as? String
        self.actionTypeId = dict["actionTypeId"] as? String
        self.extra = dict["extra"] as? [String: Any]
        self.summaryArgument = dict["summaryArgument"] as? String
        self.groupSummary = dict["groupSummary"] as? String
    }
}

@objc protocol CapacitorNotificationsExports: JSExport {
    static func schedule(_ options: JSValue)
    static func setBadge(_ options: JSValue)
    static func clearBadge()
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
        UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .sound, .badge]) { _, error in
            if let error = error {
                permissionsError = CapacitorNotificationsErrors.unknownError(reason: "\(error)")
            }

            group.leave()
        }

        group.wait()

        if let err = permissionsError {
            throw err
        }
    }

    // swiftlint:disable:next cyclomatic_complexity
    static func schedule(_ options: JSValue) {
        do {
            if CapacitorNotifications.checkPermission() != "granted" {
                throw CapacitorNotificationsErrors.permissionDenied
            }

            if options.isUndefined || options.isNull {
                throw CapacitorNotificationsErrors.invalidOptions(reason: "options are null")
            }

            if !options.isArray {
                throw CapacitorNotificationsErrors.invalidOptions(reason: "options must be an array")
            }

            if let notificationOptions = options.toArray() as? [[String: Any?]] {
                for option in notificationOptions {
                    let notificationOption = try NotificationOption(from: option)

                    let content = UNMutableNotificationContent()
                    content.title = NSString.localizedUserNotificationString(forKey: notificationOption.title, arguments: nil)
                    content.body = NSString.localizedUserNotificationString(forKey: notificationOption.body, arguments: nil)
                    content.userInfo = [:]

                    if let extra = notificationOption.extra {
                        content.userInfo["cap_extra"] = extra
                    }

                    if let actionTypeId = notificationOption.actionTypeId {
                        content.categoryIdentifier = actionTypeId
                    }

                    if let threadIdentifier = notificationOption.threadIdentifier {
                        content.threadIdentifier = threadIdentifier
                    }

                    if let summaryArgument = notificationOption.summaryArgument {
                        content.summaryArgument = summaryArgument
                    }

                    if let sound = notificationOption.sound {
                        content.sound = UNNotificationSound(named: UNNotificationSoundName(sound))
                    }

                    var notificationDate = notificationOption.scheduleAt

                    if notificationDate < Date() {
                        notificationDate = Date()
                    }

                    let dateInfo = Calendar.current.dateComponents(in: TimeZone.current, from: notificationDate)
                    // swiftlint:disable:next force_unwrapping
                    let dateInterval = DateInterval(start: Date(), end: dateInfo.date!)

                    let trigger = UNTimeIntervalNotificationTrigger(timeInterval: dateInterval.duration, repeats: false)
                    let request = UNNotificationRequest(identifier: "\(notificationOption.id)", content: content, trigger: trigger)

                    UNUserNotificationCenter.current().add(request)
                }
            }
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
        }
    }

    static func setBadge(_ options: JSValue) {
        do {
            if CapacitorNotifications.checkPermission() != "granted" {
                throw CapacitorNotificationsErrors.permissionDenied
            }

            if options.isUndefined || options.isNull {
                throw CapacitorNotificationsErrors.invalidOptions(reason: "options are null")
            }

            guard let jsonDict = options.toDictionary() as? [String: Any?] else {
                throw CapacitorNotificationsErrors.invalidOptions(reason: "options must be an valid object")
            }

            let badgeOptions = SetBadgeOption(from: jsonDict)

            DispatchQueue.main.sync {
                if #available(iOS 16.0, *) {
                    UNUserNotificationCenter.current().setBadgeCount(badgeOptions.count)
                } else {
                    UIApplication.shared.applicationIconBadgeNumber = badgeOptions.count
                }
            }
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
        }
    }

    static func clearBadge() {
        do {
            if CapacitorNotifications.checkPermission() != "granted" {
                throw CapacitorNotificationsErrors.permissionDenied
            }

            DispatchQueue.main.sync {
                if #available(iOS 16.0, *) {
                    UNUserNotificationCenter.current().setBadgeCount(0)
                } else {
                    UIApplication.shared.applicationIconBadgeNumber = 0
                }
            }
        } catch {
            JSContext.current().exception = JSValue(newErrorFromMessage: "\(error)", in: JSContext.current())
        }
    }
}
