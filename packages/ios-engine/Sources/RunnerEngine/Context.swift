import Foundation
import JavaScriptCore

public class Context {
    let name: String
    let ctx: JSContext
    
    private var timers = [Int : Timer]()
    private var eventListeners = [String : [JSValue]]()
    
    public init(vm: JSVirtualMachine, name: String) throws {
        guard let newCtx = JSContext(virtualMachine: vm) else {
            throw EngineError.jsCoreError
        }
        
        newCtx.name = name
        
        self.name = name
        self.ctx = newCtx
        
        try self.setupWebAPI()
    }
    
    public func execute(code: String) throws -> JSValue? {
        var thrownException: JSValue?
        
        self.ctx.exceptionHandler = { _, exception in
            thrownException = exception
        }
        
        let value = self.ctx.evaluateScript(code)
        
        if let exception = thrownException {
            throw EngineError.jsException(details: String(describing: exception))
        }
        
        return value
    }
    
    public func dispatchEvent(event: String, details: [String: Any]? = nil) throws {
        if let callbacks = self.eventListeners[event] {
            try callbacks.forEach { jsFunc in
                var thrownException: JSValue?
                
                self.ctx.exceptionHandler = { _, exception in
                    thrownException = exception
                }
                
                if let detailsObj = details {
                    var callbackFunctions: [String: Any] = [:]
                
                    detailsObj.keys.forEach { key in
                        if key.hasPrefix("__ebr::") {
                            let funcName = key.replacingOccurrences(of: "__ebr::", with: "")
                            callbackFunctions[funcName] = detailsObj[key]
                        }
                    }
                    
                    guard let jsDetailsObj = JSValue(object: detailsObj, in: self.ctx) else {
                        throw EngineError.jsValueError
                    }
                    
                    callbackFunctions.forEach { (funcName: String, jsFunc: Any) in
                        jsDetailsObj.setValue(jsFunc, forProperty: funcName)
                    }
                    
                    jsFunc.call(withArguments: [jsDetailsObj])
                } else {
                    jsFunc.call(withArguments: [])
                }
                
                if let exception = thrownException {
                    throw EngineError.jsException(details: String(describing: exception))
                }
            }
        }
    }
    
    
    private func setupWebAPI() throws {
        let consoleObj = JSConsole(name: self.name)
        let addEventListenerFunc: @convention(block)(String, JSValue) -> Void = { type, listener in
            return self.addEventListener(eventName: type, callback: listener)
        }
        let setTimeoutFunc: @convention(block) (JSValue, Int) -> Int = { callback, timeout in
            return self.setTimeout(callback: callback, timeout: timeout)
        }
        let setIntervalFunc: @convention(block) (JSValue, Int) -> Int = { callback, timeout in
            return self.setInterval(callback: callback, timeout: timeout)
        }
        let clearTimeoutFunc: @convention(block) (Int) -> Void = { id in
            return self.clearTimeout(id: id)
        }
        
        let newTextEncoderConst: @convention(block) () -> JSTextEncoder = JSTextEncoder.init
        let newTextDecoderConst: @convention(block) (String?, [AnyHashable: Any]?) -> JSTextDecoder = JSTextDecoder.init
        
        self.ctx.setObject(consoleObj, forKeyedSubscript: "console" as NSString)
        self.ctx.setObject(JSCrypto.self, forKeyedSubscript: "crypto" as NSString)
        self.ctx.setObject(addEventListenerFunc, forKeyedSubscript: "addEventListener" as NSString)
        self.ctx.setObject(setTimeoutFunc, forKeyedSubscript: "setTimeout" as NSString)
        self.ctx.setObject(setIntervalFunc, forKeyedSubscript: "setInterval" as NSString)
        self.ctx.setObject(clearTimeoutFunc, forKeyedSubscript: "clearTimeout" as NSString)
        self.ctx.setObject(clearTimeoutFunc, forKeyedSubscript: "clearInterval" as NSString)
        self.ctx.setObject(newTextEncoderConst, forKeyedSubscript: "TextEncoder" as NSString)
        self.ctx.setObject(newTextDecoderConst, forKeyedSubscript: "TextDecoder" as NSString)
        
    }
    
    private func addEventListener(eventName: String, callback: JSValue) {
        if self.eventListeners[eventName] == nil {
            self.eventListeners[eventName] = [callback]
        } else {
            self.eventListeners[eventName]?.append(callback)
        }
    }
    
    private func initTimer(callback: JSValue, timeout: Int, isInterval: Bool) -> Int {
        let timer = Timer.scheduledTimer(withTimeInterval: TimeInterval(timeout / 1000), repeats: isInterval) { t in
            callback.call(withArguments: [])
            if !isInterval {
                let timerId = Int(Int32.init(truncatingIfNeeded: t.hashValue))
                self.timers.removeValue(forKey: timerId)
            }
        }
        
        let timerId = Int(Int32.init(truncatingIfNeeded: timer.hashValue))
        
        RunLoop.current.add(timer, forMode: .common)
                    
        self.timers[timerId] = timer
        
        return timerId
    }
    
    private func setTimeout(callback: JSValue, timeout: Int) -> Int {
        return initTimer(callback: callback, timeout: timeout, isInterval: false)
    }
    
    private func setInterval(callback: JSValue, timeout: Int) -> Int {
        return initTimer(callback: callback, timeout: timeout, isInterval: true)
    }
    
    private func clearTimeout(id: Int) {
        if let timer = self.timers.removeValue(forKey: id) {
            timer.invalidate()
        }
    }
}
