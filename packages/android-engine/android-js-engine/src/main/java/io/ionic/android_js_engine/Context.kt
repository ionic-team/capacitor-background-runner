package io.ionic.android_js_engine

import io.ionic.android_js_engine.api.CapacitorAPI
import org.json.JSONObject

class Context(name: String, runnerPtr: Long) {
    private val ptr: Long?
    private val name: String
    private val api: ContextAPI
    private var capacitorAPI: CapacitorAPI?

    init {
        System.loadLibrary("android_js_engine")

        this.api = ContextAPI()
        this.capacitorAPI = null
        this.name = name
        this.ptr = initContext(runnerPtr, name)
    }

    external fun initContext(runnerPtr: Long, name: String): Long
    external fun destroyContext(ptr: Long)
    external fun evaluate(ptr: Long, code: String, retValue: Boolean): String
    external fun start(ptr: Long)
    external fun stop(ptr: Long)
    external fun dispatchEvent(ptr: Long, event: String, details: String)
    external fun registerGlobalFunction(ptr: Long, functionName: String,  function: JSFunction)
    external fun initCapacitorAPI(ptr: Long, api: CapacitorAPI)

    fun start() {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        start(runnerPtr)
    }

    fun stop() {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        stop(runnerPtr)
    }

    fun execute(code: String, returnValue: Boolean = false): JSValue {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        val jsonString = this.evaluate(runnerPtr, code, returnValue);

        return JSValue(jsonString)
    }

    fun dispatchEvent(event: String, details: JSONObject) {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")

        this.dispatchEvent(runnerPtr, event, details.toString(0))
    }

    fun registerFunction(funcName:String, func: JSFunction) {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        this.registerGlobalFunction(runnerPtr, funcName, func)
    }

    fun setCapacitorAPI(capAPI: CapacitorAPI) {
        val ptr = this.ptr ?: throw Exception("context pointer is null")
        this.capacitorAPI = capAPI
        this.initCapacitorAPI(ptr, this.capacitorAPI!!)
    }

    fun destroy() {
        val runnerPtr = this.ptr ?: return
        this.stop()
        this.destroyContext(runnerPtr)
    }
}