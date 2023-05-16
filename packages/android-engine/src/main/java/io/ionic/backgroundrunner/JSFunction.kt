package io.ionic.backgroundrunner
import org.json.JSONObject

open class JSFunction(args: JSONObject?) : Runnable {
    public var args: JSONObject? = null

    init {
        this.args = args
    }

    override fun run() {}
}

