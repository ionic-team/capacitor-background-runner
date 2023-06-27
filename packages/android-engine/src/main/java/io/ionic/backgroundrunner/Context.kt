package io.ionic.backgroundrunner
import org.json.JSONObject
import java.nio.charset.Charset
import java.security.SecureRandom
import java.util.*
import kotlin.math.abs

class Context(name: String, runnerPtr: Long) {
    private val ptr: Long?
    private val name: String
    private val api: ContextAPI

    init {
        System.loadLibrary("android_engine")

        this.name = name
        this.ptr = initContext(runnerPtr, name)
        this.api = ContextAPI()
    }

    external fun initContext(runnerPtr: Long, name: String): Long
    external fun destroyContext(ptr: Long)
    external fun evaluate(ptr: Long, code: String, retValue: Boolean): String
    external fun start(ptr: Long)
    external fun stop(ptr: Long)
    external fun dispatchEvent(ptr: Long, event: String, details: String)
    external fun registerGlobalFunction(ptr: Long, functionName: String,  function: JSFunction)

    fun start() {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        this.initContext(runnerPtr, this.name)
    }

    fun stop() {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        this.stop(runnerPtr)
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

    fun destroy() {
        val runnerPtr = this.ptr ?: return
        this.stop()
        this.destroyContext(runnerPtr)
    }
}
