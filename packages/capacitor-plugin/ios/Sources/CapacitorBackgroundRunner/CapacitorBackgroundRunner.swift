import Foundation
import Capacitor
import WatchConnectivity
import UserNotifications

@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin, CAPBridgedPlugin {
    public let identifier = "CapacitorBackgroundRunnerPlugin"
    public let jsName = "CapacitorBackgroundRunner"
    public let pluginMethods: [CAPPluginMethod] = [
        CAPPluginMethod(name: "checkPermissions", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "requestPermissions", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "dispatchEvent", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "registerBackgroundTask", returnType: CAPPluginReturnPromise),
        CAPPluginMethod(name: "removeNotificationListeners", returnType: CAPPluginReturnPromise)
    ]
    private let impl = BackgroundRunner()
    private weak var originalDelegate: UNUserNotificationCenterDelegate?

    override public func load() {
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(didEnterBackground),
            name: UIApplication.didEnterBackgroundNotification,
            object: nil
        )

        // Register as notification delegate
        registerNotificationCategories()

        initWatchConnectivity()
    }

    @objc override public func checkPermissions(_ call: CAPPluginCall) {
        // check geolocation permissions
        let geolocationState = CapacitorGeolocation.checkPermission()

        // check notification permissions
        let notificationState = CapacitorNotifications.checkPermission()

        call.resolve([
            "geolocation": geolocationState,
            "notifications": notificationState
        ])
    }

    @objc override public func requestPermissions(_ call: CAPPluginCall) {
        Task {
            let permissions = call.getArray("apis") as? [String] ?? []

            do {
                if permissions.contains("notifications") {
                    try CapacitorNotifications.requestPermission()
                }

                if permissions.contains("geolocation") {
                    let geolocation = CapacitorGeolocation()
                    print("geolocation requested...")
                    try await geolocation.requestPermission()
                    print("geolocation requested...done")
                }

                self.checkPermissions(call)
            } catch {
                call.reject("\(error)")
            }
        }
    }

    @objc func dispatchEvent(_ call: CAPPluginCall) {
        do {
            guard let runnerEvent = call.getString("event"), !runnerEvent.isEmpty else {
                throw BackgroundRunnerPluginError.invalidArgument(reason: "event is missing or invalid")
            }

            let details = call.getObject("details", JSObject())

            guard var config = impl.config else {
                throw BackgroundRunnerPluginError.noRunnerConfig
            }

            config.event = runnerEvent

            // swiftlint:disable:next unowned_variable_capture
            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    let result = try self.impl.execute(
                        config: config,
                        inputArgs: details as [String: Any],
                        callbackId: call.callbackId
                    )

                    if let result = result {
                        call.resolve(result)
                        return
                    }

                    call.resolve()
                } catch {
                    call.reject("\(error)")
                }
            }
        } catch {
            call.reject("\(error)")
        }
    }

    @objc func registerBackgroundTask(_ call: CAPPluginCall) {
        call.resolve()
    }

    @objc func removeNotificationListeners(_ call: CAPPluginCall) {
        notifyListeners("backgroundRunnerNotificationReceived", data: nil)
        call.resolve()
    }

    @objc private func didEnterBackground() {
        do {
            try impl.scheduleBackgroundTasks()
        } catch {
            print("could not schedule background task: \(error)")
        }
    }

    public static func registerBackgroundTask() {
        do {
            try BackgroundRunner.shared.registerBackgroundTask()
        } catch {
            print("could not register background task: \(error)")
        }
    }

    public static func dispatchEvent(event: String, eventArgs: [AnyHashable: Any], completionHandler: ((Result<Bool, Error>) -> Void)) {
        if event.isEmpty {
            completionHandler(.failure(BackgroundRunnerPluginError.invalidArgument(reason: "event is missing or invalid")))
            return
        }

        var args: [String: Any] = [:]

        eventArgs.forEach { (key: AnyHashable, value: Any) in
            if let strKey = key as? String {
                args[strKey] = value
            }
        }

        do {
            try BackgroundRunner.shared.dispatchEvent(event: event, inputArgs: args)
            completionHandler(.success(true))
        } catch {
            completionHandler(.failure(error))
        }
    }

    public static func handleApplicationDidFinishLaunching(launchOptions: [UIApplication.LaunchOptionsKey: Any]?) {
        guard let launchOptions = launchOptions else {
            return
        }

        var events: [String: [String: Any]?] = [:]

        if launchOptions[.location] != nil {
            // application was launched for a location event
            var details: [String: Any] = [:]
            details["locations"] = nil

            if let location = CapacitorGeolocation.getCurrentKnownLocation() {
                var details: [String: Any] = [:]
                details["locations"] = [location]
            }

            events["currentLocation"] = details
        }

        print("emit events: \(events)")

    }

    private func initWatchConnectivity() {
        if !WCSession.isSupported() {
            return
        }

        guard let config = impl.config else {
            return
        }

        if !config.enableWatchConnectivity {
            return
        }

        WCSession.default.delegate = self
        WCSession.default.activate()

        print("Watch Connectivity Enabled")
    }

    @objc func registerNotificationCategories() {
        let notificationCenter = UNUserNotificationCenter.current()

        // Capture the original delegate if it's not self
        if notificationCenter.delegate !== self {
            originalDelegate = notificationCenter.delegate
        }

        // Set the plugin as the notification delegate
        notificationCenter.delegate = self
    }
}

// MARK: - UNUserNotificationCenterDelegate
extension BackgroundRunnerPlugin: UNUserNotificationCenterDelegate {
    // This method will be called when a notification is tapped
    public func userNotificationCenter(_ center: UNUserNotificationCenter,
                                       didReceive response: UNNotificationResponse,
                                       withCompletionHandler completionHandler: @escaping () -> Void) {
        let userInfo = response.notification.request.content.userInfo

        if isBackgroundRunnerNotification(userInfo) {
            handleBackgroundRunnerNotification(response, completionHandler: completionHandler)
        } else {
            forwardToOriginalDelegate(center, didReceive: response, withCompletionHandler: completionHandler)
        }
    }

    // This is needed to present notifications when the app is in the foreground
    public func userNotificationCenter(_ center: UNUserNotificationCenter,
                                       willPresent notification: UNNotification,
                                       withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        let userInfo = notification.request.content.userInfo

        if isBackgroundRunnerNotification(userInfo) {
            handleBackgroundRunnerPresentation(completionHandler: completionHandler)
        } else {
            forwardToOriginalDelegate(center, willPresent: notification, withCompletionHandler: completionHandler)
        }
    }

    // MARK: - Helper Methods

    private func isBackgroundRunnerNotification(_ userInfo: [AnyHashable: Any]) -> Bool {
        return (userInfo["source"] as? String) == "capacitor-background-runner"
    }

    private func handleBackgroundRunnerNotification(_ response: UNNotificationResponse, completionHandler: @escaping () -> Void) {
        let actionTypeId = response.notification.request.content.categoryIdentifier
        let notificationId = Int(response.notification.request.identifier) ?? -1

        let eventData: [String: Any] = [
            "actionTypeId": actionTypeId,
            "notificationId": notificationId
        ]

        notifyListeners("backgroundRunnerNotificationReceived", data: eventData, retainUntilConsumed: true)
        completionHandler()
    }

    private func forwardToOriginalDelegate(_ center: UNUserNotificationCenter, didReceive response: UNNotificationResponse, withCompletionHandler completionHandler: @escaping () -> Void) {
        if let originalDelegate = originalDelegate, originalDelegate.responds(to: #selector(userNotificationCenter(_:didReceive:withCompletionHandler:))) {
            originalDelegate.userNotificationCenter?(center, didReceive: response, withCompletionHandler: completionHandler)
        } else {
            completionHandler()
        }
    }

    private func handleBackgroundRunnerPresentation(completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        if #available(iOS 14.0, *) {
            completionHandler([.banner, .sound, .badge])
        } else {
            completionHandler([.alert, .sound, .badge])
        }
    }

    private func forwardToOriginalDelegate(_ center: UNUserNotificationCenter, willPresent notification: UNNotification, withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        if let originalDelegate = originalDelegate, originalDelegate.responds(to: #selector(userNotificationCenter(_:willPresent:withCompletionHandler:))) {
            originalDelegate.userNotificationCenter?(center, willPresent: notification, withCompletionHandler: completionHandler)
        } else {
            handleBackgroundRunnerPresentation(completionHandler: completionHandler)
        }
    }
}
