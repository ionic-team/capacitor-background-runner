package io.ionic.backgroundrunner.android_engine

import org.json.JSONObject
import java.security.SecureRandom
import java.util.UUID

class Context constructor(name: String, runnerPtr: Long) {
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
        external fun evaluate(ptr: Long, code: String): JSValue
        external fun dispatchEvent(ptr: Long, event: String, details: String)

        @JvmStatic fun cryptoRandomUUID(): String {
            val random = UUID.randomUUID()
            return random.toString()
        }

        @JvmStatic fun cryptoGetRandom(size: Int): ByteArray {
            val random = SecureRandom()
            val arr = ByteArray(size)

            random.nextBytes(arr)

            return arr;
        }
    }

    fun execute(code: String): JSValue {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        return Context.evaluate(this.ptr, code)
    }

    fun dispatchEvent(event: String, details: JSONObject) {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        Context.dispatchEvent(this.ptr, event, details.toString(0))
    }

    fun destroy() {
        if (this.ptr != null) {
            Context.destroyContext(this.ptr)
        }
    }
}