package io.ionic.backgroundrunner.android_engine

sealed class EngineErrors(message: String) : Exception(message) {
    data class JavaScriptException(val details: String): EngineErrors("JS exception: $details")
}