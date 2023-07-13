package io.ionic.android_js_engine

import okhttp3.MediaType
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody
import okhttp3.RequestBody.Companion.toRequestBody
import java.lang.Exception
import java.net.URL
import java.nio.charset.Charset
import java.security.SecureRandom
import java.util.UUID
import kotlin.math.abs

class ContextAPI {
    // Crypto
    fun cryptoRandomUUID(): String {
        val random = UUID.randomUUID()
        return random.toString()
    }

    fun cryptoGetRandom(size: Int): ByteArray {
        val random = SecureRandom()
        val arr = ByteArray(size)

        random.nextBytes(arr)

        return arr;
    }

    fun randomHashCode(): Int {
        return abs(cryptoRandomUUID().hashCode())
    }

    fun stringToByteArray(str: String): ByteArray {
        return str.toByteArray(Charset.forName("UTF-8"))
    }

    fun byteArrayToString(arr: ByteArray, encoding: String): String {
        val enc = getCharset(encoding)
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

    fun fetch(urlStr: String, options: JSFetchOptions?): JSResponse {
        try {
            val url = URL(urlStr)
            val client = OkHttpClient()

            var postBody: RequestBody? = null

            val builder = Request.Builder()
            builder.url(url)

            if (options != null) {
                val contentType = options.headers["Content-Type"];

                if (options.body != null) {
                    postBody =
                        options.body!!.toRequestBody(contentType?.toMediaType(), 0, options.body!!.size)
                }

                builder.method(options.httpMethod, postBody)

                options.headers.forEach {
                    builder.addHeader(it.key, it.value)
                }
            }

            val response = client.newCall(builder.build()).execute()

            val body = response.body

            return JSResponse(response.code, response.request.url.toString(), body?.bytes(), null)
        } catch (ex: Exception) {
            print(ex.message);
            return JSResponse(-1, urlStr, null, ex.message)
        }

    }
}