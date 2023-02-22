package io.ionic.backgroundrunner.android_engine

import org.json.JSONObject

open class JSFunction constructor(args: JSONObject): Runnable {
    private val args: JSONObject
    private val ret: JSONObject? = null

    init {
        this.args = args
    }

    override fun run() {}
}