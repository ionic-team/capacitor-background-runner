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
