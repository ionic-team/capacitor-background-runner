package io.ionic.backgroundrunner.android_engine

import android.util.Log
import org.json.JSONObject
import java.nio.charset.Charset
import kotlin.math.abs
import java.security.SecureRandom
import java.util.*
import kotlin.collections.HashMap
import kotlin.concurrent.thread

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
        external fun start(ptr: Long)
        external fun stop(ptr: Long)
        external fun dispatchEvent(ptr: Long, event: String, details: String)

        @JvmStatic fun cryptoRandomUUID(): String {
            val random = UUID.randomUUID()
            return random.toString()
        }

        @JvmStatic fun randomHashCode(): Int {
            return abs(cryptoRandomUUID().hashCode())
        }

        @JvmStatic fun cryptoGetRandom(size: Int): ByteArray {
            val random = SecureRandom()
            val arr = ByteArray(size)

            random.nextBytes(arr)

            return arr;
        }

        @JvmStatic fun stringToByteArray(str: String): ByteArray {
            return str.toByteArray(Charset.forName("UTF-8"))
        }

        @JvmStatic fun byteArrayToString(arr: ByteArray, encoding: String): String {
            val enc = Context.getCharset(encoding)
            return arr.toString(enc)
        }

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
            this.stop();
            Context.destroyContext(this.ptr)
        }
    }
}