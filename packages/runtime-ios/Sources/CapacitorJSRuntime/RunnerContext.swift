import JavaScriptCore

public class RunnerContext {
    private let context: JSContext
    private var eventListeners = Dictionary<String, JSValue>()
    private var timers = Dictionary<Int, Timer>()
    
    public init(vm: JSVirtualMachine, name: String) throws {
        guard let newContext = JSContext(virtualMachine: vm) else {
            throw RuntimeError.jsCoreError
        }
        
        newContext.name = name
        newContext.exceptionHandler = { jsContext, exception in
            print("[\(name) Error] \(String(describing: exception))")
        }
        self.context = newContext
        
        try setupAPI()
    }
    
    public func run(source: String) -> JSValue?  {
        return context.evaluateScript(source)
    }
    
    public func getGlobalObject(obj: String) -> JSValue? {
        return context.objectForKeyedSubscript(obj)
    }
    
    public func emitEvent(event: String) {
        if let listener = self.eventListeners[event] {
            listener.call(withArguments: [])
        }
    }
    
    private func setupAPI() throws {
        let addEventListener: @convention(block)(String, JSValue) -> Void = { type, listener in
            print("listener callback hash \(listener.hashValue)")
            self.eventListeners[type] = listener
        }
        
        let removeEventListener: @convention(block) (String, JSValue) -> Void = { type, listener in
            self.eventListeners.removeValue(forKey: type)
        }
        
        let setTimeout: @convention(block) (JSValue, Int) -> Int = { callback, timeout in
            let timer = Timer.scheduledTimer(withTimeInterval: TimeInterval(timeout / 1000), repeats: false) { t  in
                callback.call(withArguments: [])
                self.timers.removeValue(forKey: t.hashValue)
            }
            
            RunLoop.current.add(timer, forMode: .common)
            
            self.timers[timer.hashValue] = timer
            return timer.hashValue
        }
        
        let clearTimeout: @convention(block) (Int) -> Void = { timeoutId in
            if let timer = self.timers.removeValue(forKey: timeoutId) {
                timer.invalidate()
            }
        }
        
        let setInterval: @convention(block) (JSValue, Int) -> Int = { callback, timeout in
            let timer = Timer.scheduledTimer(withTimeInterval: TimeInterval(timeout / 1000), repeats: true) { t  in
                callback.call(withArguments: [])
            }
            
            RunLoop.current.add(timer, forMode: .common)
            
            self.timers[timer.hashValue] = timer
            return timer.hashValue
        }
        
        let newCustomEvent: @convention(block) (String, [AnyHashable: Any]?) -> JSCustomEvent = JSCustomEvent.init
        
        self.context.setObject(JSConsole.self, forKeyedSubscript: "console" as NSString)
        self.context.setObject(addEventListener, forKeyedSubscript: "addEventListener" as NSString)
        self.context.setObject(removeEventListener, forKeyedSubscript: "removeEventListener" as NSString)
        self.context.setObject(setTimeout, forKeyedSubscript: "setTimeout" as NSString)
        self.context.setObject(clearTimeout, forKeyedSubscript: "clearTimeout" as NSString)
        self.context.setObject(setInterval, forKeyedSubscript: "setInterval" as NSString)
        self.context.setObject(clearTimeout, forKeyedSubscript: "clearInterval" as NSString)
        self.context.setObject(newCustomEvent, forKeyedSubscript: "CustomEvent" as NSString)
    }
}
