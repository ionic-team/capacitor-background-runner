package io.ionic.android_js_engine

sealed class EngineErrors(message: String) : Exception(message) {
    data class JavaScriptException(val details: String) : EngineErrors("JS exception: $details")

}
