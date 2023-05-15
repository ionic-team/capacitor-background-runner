import Foundation
import JavaScriptCore

public class Context {
    let name: String
    let ctx: JSContext

    private var timers = [Int: Timer]()
    private var eventListeners = [String: [JSValue]]()

    public init(vm: JSVirtualMachine, name: String) throws {
        guard let newCtx = JSContext(virtualMachine: vm) else {
            throw EngineError.jsCoreError
        }

        newCtx.name = name
        newCtx.exceptionHandler = { _, exception in
            print("[\(name) Error] \(String(describing: exception))")
        }

        self.name = name
        self.ctx = newCtx

        try setupWebAPI()
    }

    public func start() {}
    public func stop() {}

    public func execute(code: String) -> JSValue? {
        return ctx.evaluateScript(code)
    }

    public func dispatchEvent(event: String, details: [String: AnyHashable]? = nil) throws {
        if let callbacks = eventListeners[event] {
            try callbacks.forEach { jsFunc in
                if let detailsObj = details {
                    guard let jsDetailsObj = JSValue(object: detailsObj, in: ctx) else {
                        throw EngineError.jsValueError
                    }

                    jsFunc.call(withArguments: [jsDetailsObj])
                } else {
                    jsFunc.call(withArguments: [])
                }
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
        let newTextEncoderConst: @convention(block) () -> JSTextEncoder = JSTextEncoder.init
        let newTextDecoderConst: @convention(block) (String?, [AnyHashable: Any]?) -> JSTextDecoder = JSTextDecoder.init

        ctx.setObject(consoleObj, forKeyedSubscript: "console" as NSString)
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
        if eventListeners[eventName] == nil {
            eventListeners[eventName] = [callback]
        } else {
            eventListeners[eventName]?.append(callback)
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

        timers[timerId] = timer

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
