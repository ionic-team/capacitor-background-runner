package io.ionic.android_js_engine

import okhttp3.OkHttpClient
import okhttp3.Request
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

    fun fetch(urlStr: String): JSResponse {
        try {
            val url = URL(urlStr)
            val client = OkHttpClient()

            val builder = Request.Builder()
            builder.url(url)

            val response = client.newCall(builder.build()).execute()

            return JSResponse(response.code, response.request.url.toString(), response.body?.bytes())
        } catch (ex: Exception) {
            print(ex.message);
            throw ex;
        }

    }
}