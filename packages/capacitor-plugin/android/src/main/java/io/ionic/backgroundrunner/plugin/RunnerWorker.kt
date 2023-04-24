package io.ionic.backgroundrunner.plugin;

import android.content.Context
import androidx.work.Data
import androidx.work.Worker
import androidx.work.WorkerParameters
import io.ionic.backgroundrunner.JSFunction
import io.ionic.backgroundrunner.Runner
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.runBlocking
import org.json.JSONObject

class RunnerWorker(context: Context, workerParams: WorkerParameters) : Worker(context, workerParams) {
    private val runner: Runner = Runner()
    private var runnerContext: io.ionic.backgroundrunner.Context? = null

    override fun doWork(): Result {
        try {
            val label = this.inputData.getString("label") ?: ""
            val src = this.inputData.getString("src") ?: ""
            val event = this.inputData.getString("event") ?: ""

            if (label.isEmpty() || src.isEmpty() || event.isEmpty()) {
                throw Exception("label is empty")
            }

            val srcFile = this.applicationContext.assets.open("public/${src}").bufferedReader().use {
                it.readText()
            }

            this.runnerContext = this.runner.createContext(label)
            this.runnerContext?.execute(srcFile, false)

            var success = false

            runBlocking {
                success = this@RunnerWorker.execute(event)
            }

            if (success) {
                return Result.success();
            }

            return Result.failure()
        } catch (ex: Exception) {
            val data = Data.Builder()
                .putString("error", ex.toString())
                .build()

            return Result.failure(data)
        }
    }

    private suspend fun execute(event: String): Boolean {
        val result = MutableStateFlow<Boolean?>(null)
        class CompletionCallback: JSFunction(args = null) {
            override fun run() {
                super.run()
                result.value = true
            }
        }

        val callback = CompletionCallback()
        this.runnerContext?.registerFunction("_backgroundCallback", callback)


        val details = JSONObject()
        details.put("completed", "__ebr::_backgroundCallback")

        this.runnerContext?.dispatchEvent(event, details)

        val success = result.conditionalAwait {
            it != null
        }

        this.runner.destroy()

        return success!!
    }
}