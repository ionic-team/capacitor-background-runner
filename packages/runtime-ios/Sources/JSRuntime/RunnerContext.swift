import JavaScriptCore

public class RunnerContext {
    private let context: JSContext
    private var eventListeners = Dictionary<String, JSValue>()
    private var registeredEvents = Dictionary<String, [String]>()
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
    
    public func run (sourceFile: String) throws -> JSValue? {
        guard let sourceURL = Bundle.main.url(forResource: sourceFile, withExtension: "js", subdirectory: "public") else {
            throw RuntimeError.sourceNotFound
        }
        
        do {
            let source = try String(contentsOf: sourceURL)            
            return context.evaluateScript(source)
        } catch {
            throw RuntimeError.sourceNotFound
        }
    }
    
    public func getGlobalObject(obj: String) -> JSValue? {
        return context.objectForKeyedSubscript(obj)
    }
    
    public func setGlobalObject(obj: Any, forName: String) {
        context.setObject(obj, forKeyedSubscript: forName as NSString)
    }
    
    public func dispatchEvent(event: String) {
        if let registrations = self.registeredEvents[event] {
            registrations.forEach { listenerId in
                if let listener = self.eventListeners[listenerId] {
                    listener.call(withArguments: [])
                }
            }
        }
    }
    
    private func setupAPI() throws {
        let addEventListener: @convention(block)(String, JSValue) -> Void = { type, listener in
            let listenerId = "\(type)_\(listener.hashValue)"
            self.eventListeners[listenerId] = listener
            
            if var registrations = self.registeredEvents[type]{
                registrations.append(listenerId)
            } else {
                self.registeredEvents[type] = [listenerId]
            }
        }
        
        let removeEventListener: @convention(block) (String, JSValue) -> Void = { type, listener in
            let listenerId = "\(type)_\(listener.hashValue)"
            self.eventListeners.removeValue(forKey: listenerId)
            
            if var registrations = self.registeredEvents[type]{
                if let index = registrations.firstIndex(of: listenerId) {
                    registrations.remove(at: index)
                }
            }
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
        let newTextEncoder: @convention(block) () -> JSTextEncoder = JSTextEncoder.init
        let newTextDecoder: @convention(block) (String?, [AnyHashable: Any]?) -> JSTextDecoder = JSTextDecoder.init
//        let newPromise: @convention(block) (JSValue) -> JSPromise = JSPromise.init
        
        self.context.setObject(JSConsole.self, forKeyedSubscript: "console" as NSString)
        self.context.setObject(addEventListener, forKeyedSubscript: "addEventListener" as NSString)
        self.context.setObject(removeEventListener, forKeyedSubscript: "removeEventListener" as NSString)
        self.context.setObject(setTimeout, forKeyedSubscript: "setTimeout" as NSString)
        self.context.setObject(clearTimeout, forKeyedSubscript: "clearTimeout" as NSString)
        self.context.setObject(setInterval, forKeyedSubscript: "setInterval" as NSString)
        self.context.setObject(clearTimeout, forKeyedSubscript: "clearInterval" as NSString)
        self.context.setObject(newCustomEvent, forKeyedSubscript: "CustomEvent" as NSString)
        self.context.setObject(newTextEncoder, forKeyedSubscript: "TextEncoder" as NSString)
        self.context.setObject(newTextDecoder, forKeyedSubscript: "TextDecoder" as NSString)
//        self.context.setObject(newPromise, forKeyedSubscript: "Promise" as NSString)
    }
}
