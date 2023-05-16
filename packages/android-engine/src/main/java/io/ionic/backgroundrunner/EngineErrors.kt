package io.ionic.backgroundrunner

sealed class EngineErrors(message: String) : Exception(message) {
    data class JavaScriptException(val details: String) : EngineErrors("JS exception: $details")
}

