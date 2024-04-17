import Foundation
import JavaScriptCore

public class Runner {
    var machine: JSVirtualMachine

    private let runLoop: RunLoop
    private var thread: Thread?

    public init() {
        machine = JSVirtualMachine()
        runLoop = RunLoop.current
    }

    func createContext(name: String) throws -> Context {
        return try Context(vm: machine, contextName: name, runLoop: runLoop)
    }
}
