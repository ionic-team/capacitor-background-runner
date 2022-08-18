import JavaScriptCore

public class Runner {
    private var machine: JSVirtualMachine
    private var eventListeners: Dictionary<String, JSValue>
    
    public init() {
        self.machine = JSVirtualMachine()
        self.eventListeners = Dictionary<String, JSValue>()
    }
    
    private func initContext() throws -> JSContext {
        guard let newContext = JSContext(virtualMachine: self.machine) else {
            throw RuntimeError.jsCoreError
        }
        
        newContext.exceptionHandler = { jsContext, exception in
            print("[Runner Context Error] \(String(describing: exception))")
        }
        
        let addEventListener: @convention(block)(String, JSValue) -> Void = { type, listener in
            print("listener callback hash \(listener.hashValue)")
            self.eventListeners[type] = listener
        }
        
        let removeEventListener: @convention(block) (String, JSValue) -> Void = { type, listener in
            self.eventListeners.removeValue(forKey: type)
        }
        
        let newCustomEvent: @convention(block) (String) -> JSCustomEvent = JSCustomEvent.init
        
        newContext.setObject(JSConsole.self, forKeyedSubscript: "console" as NSString)
        newContext.setObject(addEventListener, forKeyedSubscript: "addEventListener" as NSString)
        newContext.setObject(removeEventListener, forKeyedSubscript: "removeEventListener" as NSString)
        
        newContext.setObject(newCustomEvent, forKeyedSubscript: "CustomEvent" as NSString)

        return newContext
        
    }
    
    public func run(sourcePath: String?, source: String?) throws {
        if sourcePath == nil && source == nil {
            throw RuntimeError.sourceNotFound
        }
        
        let context = try initContext()
        
        if let source = source {
            context.evaluateScript(source)
        }
    }
    
    public func triggerEvent(event: String) {
        let listener = self.eventListeners[event]
        listener?.call(withArguments: [])
    }
}
