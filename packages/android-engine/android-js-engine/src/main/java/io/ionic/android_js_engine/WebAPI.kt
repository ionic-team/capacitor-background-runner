package io.ionic.android_js_engine

import java.nio.charset.Charset
import kotlin.math.abs
import java.security.SecureRandom
import java.util.UUID

class WebAPI {
    companion object {
        @JvmStatic
        fun cryptoRandomUUID(): String {
            val random = UUID.randomUUID()
            return random.toString()
        }

        @JvmStatic
        fun cryptoGetRandom(size: Int): ByteArray {
            val random = SecureRandom()
            val arr = ByteArray(size)

            random.nextBytes(arr)

            return arr
        }

        @JvmStatic
        fun randomHashCode(): Int {
            return abs(cryptoRandomUUID().hashCode())
        }

        @JvmStatic
        fun stringToByteArray(str: String): ByteArray {
            return str.toByteArray(Charset.forName("UTF-8"))
        }

        @JvmStatic
        fun byteArrayToString(arr: ByteArray, encoding: String): String {
            val enc = getCharset(encoding)
            return arr.toString(enc)
        }

        @JvmStatic
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
}
