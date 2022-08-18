import JavaScriptCore

public class Runner {
    private var machine: JSVirtualMachine
    private var eventListeners = Dictionary<String, JSValue>()
    private var timers = Dictionary<Int, Timer>()
    
    public init() {
        self.machine = JSVirtualMachine()
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
        
        let newCustomEvent: @convention(block) (String) -> JSCustomEvent = JSCustomEvent.init
        
        newContext.setObject(JSConsole.self, forKeyedSubscript: "console" as NSString)
        newContext.setObject(addEventListener, forKeyedSubscript: "addEventListener" as NSString)
        newContext.setObject(removeEventListener, forKeyedSubscript: "removeEventListener" as NSString)
        newContext.setObject(setTimeout, forKeyedSubscript: "setTimeout" as NSString)
        newContext.setObject(clearTimeout, forKeyedSubscript: "clearTimeout" as NSString)
        newContext.setObject(setInterval, forKeyedSubscript: "setInterval" as NSString)
        newContext.setObject(clearTimeout, forKeyedSubscript: "clearInterval" as NSString)
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
