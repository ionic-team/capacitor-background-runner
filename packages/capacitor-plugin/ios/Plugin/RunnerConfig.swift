import Foundation
import Capacitor

public struct RunnerConfig {
    let label: String
    let src: String
    let event: String
    let repeats: Bool
    let interval: Int
    
    public init(fromJSObject: JSObject) throws {
        guard let label = fromJSObject["label"] as? String else {
            throw BackgroundRunnerPloginError.invalidRunnerConfig(reason: "runner label is missing or invalid")
        }
        
        guard let src = fromJSObject["src"] as? String else {
            throw BackgroundRunnerPloginError.invalidRunnerConfig(reason: "runner source file path is missing or invalid")
        }
        
        guard let event = fromJSObject["event"] as? String else {
            throw BackgroundRunnerPloginError.invalidRunnerConfig(reason: "runner event is missing or invalid")
        }
        
        guard let repeats = fromJSObject["repeat"] as? Bool else {
            throw BackgroundRunnerPloginError.invalidRunnerConfig(reason: "runner repeat is missing or invalid")
        }
        
        guard let interval = fromJSObject["interval"] as? Int else {
            throw BackgroundRunnerPloginError.invalidRunnerConfig(reason: "runner interval is missing or invalid")
        }
        
        self.label = label
        self.src = src
        self.event = event
        self.repeats = repeats
        self.interval = interval
    }
}
