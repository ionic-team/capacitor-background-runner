package io.ionic.android_js_engine

import org.json.JSONObject

class JSResponse(statusCode: Int, url: String, data: ByteArray?) {
    public var ok: Boolean
    public var status: Int
    public var url: String

    private var data: ByteArray?

    init {
        this.status = statusCode
        this.ok = statusCode in 201..299
        this.url = url
        this.data = data
    }

    fun text(): String {
        val data = this.data ?: return ""
        return data.toString()
    }

    fun json(): JSONObject? {
        val data = this.data ?: return null

        return JSONObject(data.toString());
    }
}