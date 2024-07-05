import Foundation
import Capacitor
import WatchConnectivity

@objc(BackgroundRunnerPlugin)
public class BackgroundRunnerPlugin: CAPPlugin {
    private let impl = BackgroundRunner()

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
}
