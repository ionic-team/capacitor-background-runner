package io.ionic.android_js_engine

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
    private external fun registerGlobalFunction(ptr: Long, functionName: String, function: NativeJSFunction)
//    private external fun dispatchEvent(ptr: Long, event: String, details: String)
//    private external fun setCapacitorAPI(ptr: Long, api: CapacitorAPI)

    fun execute(code: String, returnValue: Boolean = false): NativeJSValue {
        val contextPtr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        val jsonString = this.evaluate(contextPtr, code, returnValue)

        return NativeJSValue(jsonString)
    }

    fun registerFunction(jsFunctionName: String, func: NativeJSFunction) {
        val contextPtr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        this.registerGlobalFunction(contextPtr, jsFunctionName, func)
    }

    internal fun destroy() {
        val contextPtr = this.ptr ?: throw EngineErrors.ContextException("pointer is nil")
        destroyRunnerContext(runnerPtr, contextPtr)
    }
}