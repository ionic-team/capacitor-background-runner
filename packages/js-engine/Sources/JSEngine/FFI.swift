import Foundation

// @_cdecl("jsengine_create_runner")
// public func createRunner() -> OpaquePointer {
//   // do something, then return pointer to object
//   return 0
// }

@_cdecl("jsengine_check_for_pending_jobs")
public func checkForPendingJobs(ptr: OpaquePointer) -> CBool {
  return false
}

@_cdecl("jsengine_execute_pending_jobs")
public func executePendingJobs(ptr: OpaquePointer) {}

@_cdecl("jsengine_destroy_runner")
public func destroyRunner(ptr: OpaquePointer) {
}

// @_cdecl("jsengine_create_context")
// public func createContext(runnerPtr: OpaquePointer) -> OpaquePointer {
//   return 0
// }

@_cdecl("jsengine_destroy_context")
public func destroyRunnerContext(ptr: OpaquePointer) {}

@_cdecl("jsengine_evaluate")
public func evaluate(ptr: OpaquePointer, code: UnsafePointer<CChar>?) -> UnsafeMutablePointer<
  UInt8
>? {
  return nil
}

@_cdecl("jsengine_register_function")
public func registerFunction(ptr: OpaquePointer, name: UnsafePointer<CChar>?) {}

@_cdecl("jsengine_dispatch_event")
public func dispatchEvent(
  ptr: OpaquePointer, event: UnsafePointer<CChar>?, details: UnsafePointer<CChar>?
) {}

@_cdecl("jsengine_free_data_buffer")
public func freeDataBuffer(ptr: UnsafeMutablePointer<UInt8>?) {
  free(ptr)
}
