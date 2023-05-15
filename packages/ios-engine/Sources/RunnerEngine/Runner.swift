import Foundation
import JavaScriptCore

public class Runner {
    var machine: JSVirtualMachine
    var contexts: [String: Context]

    public init() {
        self.machine = JSVirtualMachine()
        self.contexts = [:]
    }

    func createContext(name: String) throws -> Context {
        let ctx = try Context(vm: self.machine, name: name)

        self.contexts[name] = ctx

        return ctx
    }

    func destroyContext(name: String) {
        self.contexts.removeValue(forKey: name)
    }
}
