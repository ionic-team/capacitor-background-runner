package io.ionic.backgroundrunner
import org.json.JSONObject
import java.nio.charset.Charset
import java.security.SecureRandom
import java.util.*
import kotlin.math.abs

class Context(name: String, runnerPtr: Long) {
    private val ptr: Long?
    private val name: String

    init {
        this.name = name
        this.ptr = Context.initContext(runnerPtr, name)
    }

    companion object {
        init {
            System.loadLibrary("android_engine")
        }

        external fun initContext(runnerPtr: Long, name: String): Long
        external fun destroyContext(ptr: Long)
        external fun evaluate(ptr: Long, code: String, retValue: Boolean): String
        external fun start(ptr: Long)
        external fun stop(ptr: Long)
        external fun dispatchEvent(ptr: Long, event: String, details: String)
        external fun registerGlobalFunction(ptr: Long, functionName: String, function: JSFunction)

        @JvmStatic
        fun cryptoRandomUUID(): String {
            val random = UUID.randomUUID()
            return random.toString()
        }

        @JvmStatic
        fun randomHashCode(): Int {
            return abs(cryptoRandomUUID().hashCode())
        }

        @JvmStatic
        fun cryptoGetRandom(size: Int): ByteArray {
            val random = SecureRandom()
            val arr = ByteArray(size)

            random.nextBytes(arr)

            return arr
        }

        @JvmStatic
        fun stringToByteArray(str: String): ByteArray {
            return str.toByteArray(Charset.forName("UTF-8"))
        }

        @JvmStatic
        fun byteArrayToString(arr: ByteArray, encoding: String): String {
            val enc = Context.getCharset(encoding)
            return arr.toString(enc)
        }

        @JvmStatic
        fun KV_Set(key: String, value: String) {}

        @JvmStatic
        fun KV_Get(key: String): String {
            return ""
        }

        @JvmStatic
        fun KV_Remove(key: String) {}

        fun getCharset(encoding: String): Charset {
            if (encoding == "utf-16be") {
                return Charsets.UTF_16BE
            }

            if (encoding == "utf-16le") {
                return Charsets.UTF_16LE
            }

            if (encoding == "utf-8") {
                return Charsets.UTF_8
            }

            return Charset.forName(encoding)
        }
    }

    fun start() {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        Context.start(this.ptr)
    }

    fun stop() {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        Context.stop(this.ptr)
    }

    fun execute(code: String, returnValue: Boolean = false): JSValue {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        val jsonString = Context.evaluate(this.ptr, code, returnValue)

        return JSValue(jsonString)
    }

    fun dispatchEvent(event: String, details: JSONObject) {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        Context.dispatchEvent(this.ptr, event, details.toString(0))
    }

    fun registerFunction(funcName: String, func: JSFunction) {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        Context.registerGlobalFunction(this.ptr, funcName, func)
    }

    fun destroy() {
        if (this.ptr != null) {
            this.stop()
            Context.destroyContext(this.ptr)
        }
    }
}
