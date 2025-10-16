import CQuickJS
import Foundation

class Context {
  private let _context: OpaquePointer?

  init(runner: Runner) {
    self._context = JS_NewContext(runner.pointer())
  }

  func evaluate(code: String) -> Data? {
    let codeCStr = strdup(code)

    let flags = JS_EVAL_TYPE_GLOBAL | JS_EVAL_FLAG_BACKTRACE_BARRIER

    let ret = JS_Eval(_context, codeCStr, code.count, "<code>", flags)
    defer {
      JS_FreeValue(_context, ret)
    }

    if JS_IsException(ret) != 0 {
      //throw an exception
    }

    let json = JS_JSONStringify(_context, ret, SWIFT_JS_UNDEFINED, SWIFT_JS_UNDEFINED)
    let jsoncStr = JS_ToCString(_context, json)

    guard let jsonStrPtr = jsoncStr else {
      return nil
    }

    let jsonString = String(cString: jsonStrPtr)

    print(jsonString)

    return nil
  }

  deinit {
    JS_FreeContext(_context)
  }
}
