import Foundation
import Capacitor

public struct RunnerConfig {
    let label: String
    let src: String
    var autoSchedule: Bool
    var event: String
    let repeats: Bool
    let enableWatchConnectivity: Bool

    let interval: Int?

    public init(from jsObject: JSObject) throws {
        guard let label = jsObject["label"] as? String else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner label is missing or invalid")
        }

        guard let src = jsObject["src"] as? String else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner source file path is missing or invalid")
        }

        let event = jsObject["event"] as? String ?? ""
        let repeats = jsObject["repeat"] as? Bool
        let interval = jsObject["interval"] as? Int
        let autoStart = jsObject["autoStart"] as? Bool

        self.label = label
        self.src = src
        self.event = event
        self.repeats = repeats ?? false
        self.interval = interval
        self.autoSchedule = autoStart ?? false
        self.enableWatchConnectivity = jsObject["enableWatchConnectivity"] as? Bool ?? false
    }
}
