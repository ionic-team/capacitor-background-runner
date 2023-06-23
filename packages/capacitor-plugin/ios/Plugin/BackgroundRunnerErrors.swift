import Foundation

public enum BackgroundRunnerPluginError: Error, Equatable {
    case invalidRunnerConfig(reason: String)
    case invalidArgument(reason: String)
    case runnerError(reason: String)
    case genericError(reason: String)
}
