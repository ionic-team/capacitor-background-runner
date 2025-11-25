package io.ionic.android_js_engine

import org.json.JSONObject

open class NativeJSFunction(
    jsFunctionName: String,
    jsFunctionArgs: JSONObject? = null,
) : Runnable {
    var jsFunctionArgs: JSONObject? = null
    var jsFunctionName: String

    init {
        this.jsFunctionName = jsFunctionName
        this.jsFunctionArgs = jsFunctionArgs
    }

    override fun run() {}
}
