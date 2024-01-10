package io.ionic.android_js_engine

sealed class EngineErrors(message: String): Exception(message) {
    data class JavaScriptException(val details: String) : EngineErrors("JS exception: $details")
    data class RunnerException(val details: String) : EngineErrors("Runner exception: $details")
    data class ContextException(val details: String) : EngineErrors("Context exception: $details")
}