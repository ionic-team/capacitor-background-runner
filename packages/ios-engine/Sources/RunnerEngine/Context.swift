import Foundation
import JavaScriptCore

public class Context {
    let name: String
    let ctx: JSContext

    private var timers = [Int: Timer]()
    private var eventListeners = [String: JSValue]()
    private var thread: Thread!
    private var runLoop: RunLoop!

    // swiftlint:disable:next identifier_name
    public init(vm: JSVirtualMachine, ctxName: String) throws {
        guard let newCtx = JSContext(virtualMachine: vm) else {
            throw EngineError.jsCoreError
        }

        newCtx.name = ctxName

        name = ctxName
        ctx = newCtx
        
        thread = Thread { [weak self] in
            self?.runLoop = RunLoop.current
            
            while (self != nil && !self!.thread.isCancelled) {
                self?.runLoop.run(mode: .default, before: .distantFuture)
            }
            
            Thread.exit()
        }
        
        thread.name = "[\(name)] context thread"
        thread.qualityOfService = .userInitiated
        thread.start()

        try setupWebAPI()
    }

    public func execute(code: String) throws -> JSValue? {
        var thrownException: JSValue?

        ctx.exceptionHandler = { _, exception in
            thrownException = exception
        }

        let value = ctx.evaluateScript(code)

        if let exception = thrownException {
            throw EngineError.jsException(details: String(describing: exception))
        }

        return value
    }

    public func dispatchEvent(event: String, details: [String: Any]? = nil) throws {
        if let eventHandler = eventListeners[event] {
            let dataArgs = details?["dataArgs"] as? [String: Any]
            var callbackFunctions: [String: JSValue] = [:]
            
            if let callbacks = details?["callbacks"] as? [String: Any] {
                callbacks.keys.forEach { key in
                    if key.hasPrefix("__cbr::") {
                        let funcName = key.replacingOccurrences(of: "__cbr::", with: "")
                        callbackFunctions[funcName] = JSValue.init(object: callbacks[key], in: self.ctx)
                    }
                }
            }
        
            var thrownException: JSValue?
            
            self.ctx.exceptionHandler = { _, exception in
                thrownException = exception
            }
            
            let resolveFunc = callbackFunctions["resolve"]
            let rejectFunc = callbackFunctions["reject"]
            
            if let dataArgs = dataArgs {
                guard let jsDataArgs = JSValue(object: dataArgs, in: self.ctx) else {
                    throw EngineError.jsValueError
                }
                
                eventHandler.call(withArguments: [resolveFunc!, rejectFunc!, jsDataArgs])
            } else {
                eventHandler.call(withArguments: [resolveFunc!, rejectFunc as Any])
            }
            
            if let exception = thrownException {
                throw EngineError.jsException(details: String(describing: exception))
            }
            
        }
    }
    private func setupWebAPI() throws {
        let consoleObj = JSConsole(name: name)
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
        let fetchFunc: @convention(block) (JSValue, JSValue) -> JSValue = { resource, options in
            return fetch(resource: resource, options: options)
        }
    
        let newTextEncoderConst: @convention(block) () -> JSTextEncoder = JSTextEncoder.init
        let newTextDecoderConst: @convention(block) (String?, [AnyHashable: Any]?) -> JSTextDecoder = JSTextDecoder.init

        ctx.setObject(consoleObj, forKeyedSubscript: "console" as NSString)
        ctx.setObject(fetchFunc, forKeyedSubscript: "fetch" as NSString)
        ctx.setObject(JSCrypto.self, forKeyedSubscript: "crypto" as NSString)
        ctx.setObject(addEventListenerFunc, forKeyedSubscript: "addEventListener" as NSString)
        ctx.setObject(setTimeoutFunc, forKeyedSubscript: "setTimeout" as NSString)
        ctx.setObject(setIntervalFunc, forKeyedSubscript: "setInterval" as NSString)
        ctx.setObject(clearTimeoutFunc, forKeyedSubscript: "clearTimeout" as NSString)
        ctx.setObject(clearTimeoutFunc, forKeyedSubscript: "clearInterval" as NSString)
        ctx.setObject(newTextEncoderConst, forKeyedSubscript: "TextEncoder" as NSString)
        ctx.setObject(newTextDecoderConst, forKeyedSubscript: "TextDecoder" as NSString)

    }

    private func addEventListener(eventName: String, callback: JSValue) {
        eventListeners[eventName] = callback
    }

    private func initTimer(callback: JSValue, timeout: Int, isInterval: Bool) -> Int {
        let timer = Timer(timeInterval: TimeInterval(timeout / 1000), repeats: isInterval) { t in
            callback.call(withArguments: [])
            if !isInterval {
                let timerId = Int(Int32.init(truncatingIfNeeded: t.hashValue))
                self.timers.removeValue(forKey: timerId)
            }
        }

        let timerId = Int(Int32.init(truncatingIfNeeded: timer.hashValue))
        timers[timerId] = timer
        
        self.runLoop.add(timer, forMode: .default)
        self.runLoop.add(timer, forMode: .common)

        return timerId
    }

    private func setTimeout(callback: JSValue, timeout: Int) -> Int {
        return initTimer(callback: callback, timeout: timeout, isInterval: false)
    }

    private func setInterval(callback: JSValue, timeout: Int) -> Int {
        return initTimer(callback: callback, timeout: timeout, isInterval: true)
    }

    private func clearTimeout(id: Int) {
        if let timer = timers.removeValue(forKey: id) {
            timer.invalidate()
        }
    }
}
