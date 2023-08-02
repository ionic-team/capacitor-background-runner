import Foundation
import Capacitor
import WatchConnectivity
import OSLog

@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin {
    private let impl = BackgroundRunner.shared

    override public func load() {
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(didEnterBackground),
            name: UIApplication.didEnterBackgroundNotification,
            object: nil
        )
        
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
                    try await geolocation.requestPermission()                    
                }

                self.checkPermissions(call)
            } catch {
                call.reject("\(error)")
            }
        }
    }

    @objc func dispatchEvent(_ call: CAPPluginCall) {
        do {
            guard let runnerEvent = call.getString("event") else {
                throw BackgroundRunnerPluginError.invalidArgument(reason: "event is missing or invalid")
            }

            let details = call.getObject("details", JSObject())

            guard let config = impl.getConfig() else {
                throw BackgroundRunnerPluginError.runnerError(reason: "no runner config loaded")
            }

            DispatchQueue.global(qos: .userInitiated).async { [unowned self] in
                do {
                    let result = try self.impl.execute(
                        config: config,
                        event: runnerEvent,
                        inputArgs: details as [String: Any]
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

    @objc private func didEnterBackground() {
        impl.scheduleBackgroundTasks()
    }

    public static func registerBackgroundTask() {
        BackgroundRunner.shared.registerBackgroundTask()
    }

    public static func dispatchEvent(event: String, eventArgs: [AnyHashable: Any], completionHandler: ((Result<Bool, Error>) -> Void)) {
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
    }

    private func initWatchConnectivity() {
        let logger = Logger(subsystem: "BackgroundRunnerPlugin", category: "WatchConnectivity")

        if !WCSession.isSupported() {
            return
        }

        guard let config = impl.getConfig() else {
            return
        }

        if !config.enableWatchConnectivity {
            return
        }

        WCSession.default.delegate = self
        WCSession.default.activate()

        logger.debug("BackgroundRunner + Watch Connectivity Enabled")        
    }
}
