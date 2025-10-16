import CQuickJS

class Runner {
  private let _rt: OpaquePointer?

  init() {
    self._rt = JS_NewRuntime()
    JS_SetCanBlock(self._rt, 0)
    JS_SetMaxStackSize(self._rt, 0)

    print("Runner created")
  }

  func pointer() -> OpaquePointer? {
    _rt
  }

  deinit {
    JS_FreeRuntime(_rt)
    print("Runner destroyed")
  }
}
