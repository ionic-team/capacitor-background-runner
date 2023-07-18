package io.ionic.android_js_engine

import org.json.JSONObject
import java.nio.charset.Charset

class JSFetchOptions(jsonString: String) {
    public val httpMethod: String
    public val headers = HashMap<String, String>()
    public var body: ByteArray? = null

    init {
        val jsObject = JSONObject(jsonString)

        val bodyString = jsObject.getString("body")
        if (!bodyString.isNullOrEmpty()) {
            body = bodyString.toByteArray(Charset.defaultCharset())
        }

        val jsHeaders = jsObject.getJSONObject("headers")
        jsHeaders.keys().forEach {
            headers[it] = jsHeaders.getString(it)
        }

        httpMethod = jsObject.optString("method", "GET").uppercase()
    }
}
