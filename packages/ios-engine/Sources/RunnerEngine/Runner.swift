import Foundation
import JavaScriptCore

public class Runner {
    var machine: JSVirtualMachine
    var contexts: [String: Context]
    public init() {
        machine = JSVirtualMachine()
        contexts = [:]
    }

    func createContext(name: String) throws -> Context {
        let ctx = try Context(vm: machine, ctxName: name)

        contexts[name] = ctx

        return ctx
    }

    func destroyContext(name: String) {
        contexts.removeValue(forKey: name)
    }
}
