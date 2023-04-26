package io.ionic.backgroundrunner.plugin

import android.content.Context
import io.ionic.backgroundrunner.JSFunction
import io.ionic.backgroundrunner.Runner
import kotlinx.coroutines.flow.MutableStateFlow
import org.json.JSONObject

suspend fun executeRunner(config: RunnerConfig, context: Context, args: JSONObject, overrideEvent: String? = null): JSONObject? {
    val srcFile = context.assets.open("public/${config.src}").bufferedReader().use {
        it.readText()
    }

    val runner = Runner()
    val runnerContext = runner.createContext(config.label)
    runnerContext.execute(srcFile, false)

    val result = MutableStateFlow<Result<JSONObject?>?>(null)

    class CompletionCallback: JSFunction(args = null) {
        override fun run() {
            super.run()
            result.value = Result.success(this.args)
        }
    }

    val callback = CompletionCallback()
    runnerContext.registerFunction("_backgroundCallback", callback)

    args.put("completed", "__ebr::_backgroundCallback")

    if (overrideEvent != null) {
        runnerContext.dispatchEvent(overrideEvent, args)
    } else {
        runnerContext.dispatchEvent(config.event, args)
    }

    val finished = result.conditionalAwait {
        it != null
    }

    runner.destroy()

    if (finished!!.isSuccess) {
        return finished.getOrNull()
    } else {
        val ex = finished.exceptionOrNull()
        if (ex != null) {
            throw ex
        } else {
            throw Exception("unknown exception")
        }
    }
}