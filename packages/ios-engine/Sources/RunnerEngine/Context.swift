import Foundation
import JavaScriptCore

public class Context {
    let name: String
    let jsContext: JSContext

    private var eventListeners = [String: JSValue]()
    private var timers = [Int: Timer]()
    private weak var runLoop: RunLoop?

    // swiftlint:disable:next identifier_name
    public init(vm: JSVirtualMachine, contextName: String, runLoop: RunLoop) throws {
        guard let jsContext = JSContext(virtualMachine: vm) else {
            throw EngineError.jsCoreError
        }

        self.name = contextName
        self.jsContext = jsContext
        self.runLoop = runLoop

        try setupWebAPI()
    }

    public func execute(code: String) throws -> JSValue? {
        var thrownException: JSValue?

        jsContext.exceptionHandler = { _, exception in
            thrownException = exception
        }

        let value = jsContext.evaluateScript(code)

        if let exception = thrownException {
            throw EngineError.jsException(details: String(describing: exception))
        }

        return value
    }

    public func dispatchEvent(event: String, details: [String: Any]? = nil) throws {
        if let eventHandler = eventListeners[event] {
            var dataArgs = details?["dataArgs"] as? [String: Any]
            if dataArgs == nil {
                dataArgs = details
            }

            var callbackFunctions: [String: JSValue] = [:]

            if let callbacks = details?["callbacks"] as? [String: Any] {
                callbacks.keys.forEach { key in
                    if key.hasPrefix("__cbr::") {
                        let funcName = key.replacingOccurrences(of: "__cbr::", with: "")
                        callbackFunctions[funcName] = JSValue.init(object: callbacks[key], in: self.jsContext)
                    }
                }
            }

            var thrownException: JSValue?

            self.jsContext.exceptionHandler = { _, exception in
                thrownException = exception
            }

            var callArgs: [Any] = []

            if let resolveFunc = callbackFunctions["resolve"],
               let rejectFunc = callbackFunctions["reject"] {
                callArgs.append(resolveFunc)
                callArgs.append(rejectFunc)
            }

            if let dataArgs = dataArgs {
                guard let jsDataArgs = JSValue(object: dataArgs, in: self.jsContext) else {
                    throw EngineError.jsValueError
                }

                callArgs.append(jsDataArgs)
            }

            eventHandler.call(withArguments: callArgs)

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

        jsContext.setObject(consoleObj, forKeyedSubscript: "console" as NSString)
        jsContext.setObject(fetchFunc, forKeyedSubscript: "fetch" as NSString)
        jsContext.setObject(JSCrypto.self, forKeyedSubscript: "crypto" as NSString)
        jsContext.setObject(addEventListenerFunc, forKeyedSubscript: "addEventListener" as NSString)
        jsContext.setObject(setTimeoutFunc, forKeyedSubscript: "setTimeout" as NSString)
        jsContext.setObject(setIntervalFunc, forKeyedSubscript: "setInterval" as NSString)
        jsContext.setObject(clearTimeoutFunc, forKeyedSubscript: "clearTimeout" as NSString)
        jsContext.setObject(clearTimeoutFunc, forKeyedSubscript: "clearInterval" as NSString)
        jsContext.setObject(newTextEncoderConst, forKeyedSubscript: "TextEncoder" as NSString)
        jsContext.setObject(newTextDecoderConst, forKeyedSubscript: "TextDecoder" as NSString)

    }

    private func addEventListener(eventName: String, callback: JSValue) {
        eventListeners[eventName] = callback
    }

    private func initTimer(callback: JSValue, timeout: Int, isInterval: Bool) -> Int {
        let timer = Timer(timeInterval: TimeInterval(timeout / 1000), repeats: isInterval) { [weak self] t in
            guard let self = self else { return }

            callback.call(withArguments: [])
            if !isInterval {
                let timerId = Int(Int32.init(truncatingIfNeeded: t.hashValue))
                self.timers.removeValue(forKey: timerId)
            }
        }

        let timerId = Int(Int32.init(truncatingIfNeeded: timer.hashValue))
        timers[timerId] = timer

        if let runLoop = self.runLoop {
            runLoop.add(timer, forMode: .default)
            runLoop.add(timer, forMode: .common)
        }

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
