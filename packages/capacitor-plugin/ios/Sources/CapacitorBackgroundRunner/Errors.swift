import Foundation

public enum BackgroundRunnerPluginError: Error, Equatable {
    case noRunnerConfig
    case invalidRunnerConfig(reason: String)
    case invalidArgument(reason: String)
    case runnerError(reason: String)
    case genericError(reason: String)
}
