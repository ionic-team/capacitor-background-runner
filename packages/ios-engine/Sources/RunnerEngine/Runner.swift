import Foundation
import JavaScriptCore

public class Runner {
    var machine: JSVirtualMachine
    var contexts: [String: Context]

    private let runLoop: RunLoop
    private var thread: Thread?

    public init() {
        machine = JSVirtualMachine()
        contexts = [:]
        runLoop = RunLoop.current
    }

    func createContext(name: String) throws -> Context {
        let context = try Context(vm: machine, contextName: name, runLoop: runLoop)

        if contexts.keys.contains(name) {
            throw EngineError.runnerError(details: "context with name \(name) already exists")
        }

        contexts[name] = context

        return context
    }

    func destroyContext(name: String) {
        contexts.removeValue(forKey: name)
    }

    func start() {
        thread = Thread { [weak self] in
            defer {
                Thread.exit()
            }

            guard let self = self else { return }
            guard let thread = self.thread else { return }

            while !thread.isCancelled {
                self.runLoop.run(mode: .default, before: .distantFuture)
            }
        }

        thread?.qualityOfService = .userInitiated
        thread?.start()
    }

    func stop() {
        guard let thread = self.thread else {
            return
        }

        thread.cancel()
        while !thread.isFinished {
            Thread.sleep(forTimeInterval: 0.05)
        }

        self.thread = nil
    }
}
