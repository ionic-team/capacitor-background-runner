import Foundation

public enum EngineError: Error {
    case jsCoreError
    case jsValueError
    case sourceNotFound
    case jsException(details: String)
    case runnerError(details: String)
}
