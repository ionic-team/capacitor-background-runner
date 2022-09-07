import JavaScriptCore

public class Runner {
    private var machine: JSVirtualMachine
    private var contexts = Dictionary<String, RunnerContext>()
    
    public init() {
        self.machine = JSVirtualMachine()
    }
    
    public func createContext(name: String) throws -> RunnerContext {
        let context = try RunnerContext(vm: self.machine, name: name)
        self.contexts[name] = context
        
        return context
    }
    
    public func destroyContext(name: String) {
        self.contexts.removeValue(forKey: name)
    }
    
    public func execute(name: String, code: String) -> JSValue? {
        guard let context = self.contexts[name] else {
            return nil
        }
        
        return context.run(source: code)
    }
    
    public func execute(name: String, sourcePath: String) -> JSValue? {
        guard let context = self.contexts[name] else {
            return nil
        }
        
        return try? context.run(sourceFile: sourcePath)
    }
    
    public func dispatchEvent(event: String) {
        for (_, context) in self.contexts {
            context.dispatchEvent(event: event)
        }
    }
}
