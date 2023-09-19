package io.ionic.android_js_engine

import io.ionic.android_js_engine.api.CapacitorAPI
import org.json.JSONObject

class Context(name: String, runnerPtr: Long) {
    val name: String
    private val runnerPtr: Long

    private var ptr: Long? = null

    init {
        System.loadLibrary("android_js_engine")
        this.name = name
        this.runnerPtr = runnerPtr
        this.ptr = createRunnerContext(runnerPtr, name)
    }

    private external fun createRunnerContext(runnerPtr: Long, name: String): Long
    private external fun destroyRunnerContext(runnerPtr: Long, ptr: Long)
    private external fun evaluate(ptr: Long, code: String, retValue: Boolean): String
    private external fun registerGlobalFunction(ptr: Long, functionName: String, function: JSFunction)
    private external fun dispatchEvent(ptr: Long, event: String, details: String)
    private external fun setCapacitorAPI(ptr: Long, api: CapacitorAPI)

    fun execute(code: String, returnValue: Boolean = false): JSValue {
        val ptr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        val jsonString = this.evaluate(ptr, code, returnValue)

        return JSValue(jsonString)
    }

    fun registerFunction(funcName: String, func: JSFunction) {
        val ptr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        this.registerGlobalFunction(ptr, funcName, func)
    }

    fun dispatchEvent(event: String, details: JSONObject) {
        val ptr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        this.dispatchEvent(ptr, event, details.toString(0))
    }

    fun setCapacitorAPI(capAPI: CapacitorAPI) {
        val ptr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        this.setCapacitorAPI(ptr, capAPI)
    }

    internal fun destroy() {
        val contextPtr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        destroyRunnerContext(runnerPtr, contextPtr);
    }
}