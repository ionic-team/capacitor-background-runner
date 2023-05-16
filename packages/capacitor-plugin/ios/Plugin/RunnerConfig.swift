import Foundation
import Capacitor

public struct RunnerConfig {
    let label: String
    let src: String
    let event: String
    let repeats: Bool
    var autoStart: Bool
    let interval: Int
    
    public init(from jsObject: JSObject) throws {
        guard let label = jsObject["label"] as? String else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner label is missing or invalid")
        }
        
        guard let src = jsObject["src"] as? String else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner source file path is missing or invalid")
        }
        
        guard let event = jsObject["event"] as? String else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner event is missing or invalid")
        }
        
        guard let repeats = jsObject["repeat"] as? Bool else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner repeat is missing or invalid")
        }
        
        guard let interval = jsObject["interval"] as? Int else {
            throw BackgroundRunnerPluginError.invalidRunnerConfig(reason: "runner interval is missing or invalid")
        }
        
        let autoStart = jsObject["autoStart"] as? Bool
        
        self.label = label
        self.src = src
        self.event = event
        self.repeats = repeats
        self.interval = interval
        self.autoStart = autoStart ?? false
    }
}
