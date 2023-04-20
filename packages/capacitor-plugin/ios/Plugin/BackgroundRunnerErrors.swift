import Foundation

public enum BackgroundRunnerPloginError: Error, Equatable {
    case invalidRunnerConfig(reason: String)
    case runnerError(reason: String)
}
