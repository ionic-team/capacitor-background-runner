package io.ionic.android_js_engine

import android.util.Log
import java.io.BufferedInputStream
import java.io.BufferedOutputStream
import java.net.HttpURLConnection
import java.net.URL
import java.nio.charset.Charset
import java.security.SecureRandom
import java.util.UUID
import kotlin.math.abs

class NativeWebAPI {
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
        fun byteArrayToString(
            arr: ByteArray,
            encoding: String,
        ): String {
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

        @JvmStatic
        fun fetch(
            urlStr: String,
            options: NativeJSFetchOptions?,
        ): NativeJSResponse {
            val url = URL(urlStr)
            val connection = url.openConnection() as HttpURLConnection

            try {
                if (options != null) {
                    connection.requestMethod = options.httpMethod

                    options.headers.forEach {
                        connection.setRequestProperty(it.key, it.value)
                    }

                    val requestBody = options.body
                    if (requestBody != null) {
                        connection.doOutput = true
                        val contentEncoding = options.headers["Transfer-Encoding"]
                        if (contentEncoding == "chunked") {
                            connection.setChunkedStreamingMode(0)
                        } else {
                            connection.setFixedLengthStreamingMode(requestBody.size)
                        }

                        val output = BufferedOutputStream(connection.outputStream)
                        output.write(requestBody)
                        output.flush()
                    }
                }

                val input = BufferedInputStream(connection.inputStream)
                val bodyBytes = input.readBytes()

                return NativeJSResponse(connection.responseCode, connection.url.toString(), bodyBytes, null)
            } catch (ex: Exception) {
                Log.e("KOTLIN FETCH ERROR", ex.toString())
                return NativeJSResponse(-1, urlStr, null, ex.message)
            } finally {
                connection.disconnect()
            }
        }
    }
}
