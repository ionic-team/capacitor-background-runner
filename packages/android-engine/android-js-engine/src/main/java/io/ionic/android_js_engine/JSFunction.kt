package io.ionic.android_js_engine

import org.json.JSONObject

open class JSFunction(jsName: String, args: JSONObject? = null) : Runnable {
    public var args: JSONObject? = null
    public var name: String

    init {
        this.name = jsName
        this.args = args
    }

    override fun run() {}
}
