package io.ionic.backgroundrunner.plugin;

import io.ionic.backgroundrunner.JSFunction
import io.ionic.backgroundrunner.Runner
import androidx.work.Data
import androidx.work.OneTimeWorkRequest
import androidx.work.PeriodicWorkRequest
import androidx.work.WorkManager
import com.getcapacitor.JSObject
import com.getcapacitor.Plugin
import com.getcapacitor.PluginCall
import com.getcapacitor.PluginMethod
import com.getcapacitor.annotation.CapacitorPlugin
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import org.json.JSONObject
import java.util.concurrent.TimeUnit

@CapacitorPlugin(name = "BackgroundRunner")
class BackgroundRunnerPlugin: Plugin() {
    private var configs: HashMap<String, RunnerConfig> = HashMap()

    override fun load() {
        val runnerConfigs = config.configJSON.getJSONArray("runners")

        for (i in 0 until runnerConfigs.length()) {
            val configJSON = runnerConfigs.getJSONObject(i)

            val runnerConfig = RunnerConfig(
                configJSON.getString("label"),
                configJSON.getString("src"),
                configJSON.getString("event"),
                configJSON.getBoolean("repeat"),
                configJSON.getInt("interval"),
            )

            configs[runnerConfig.label] = runnerConfig
        }

        this.initRunnerWorkers()
    }

    @PluginMethod
    fun dispatchEvent(call: PluginCall) {
        val runnerLabel = call.getString("label") ?: throw Exception("label is missing")
        val runnerEvent = call.getString("event") ?: throw Exception("event is missing")

        val details = call.getObject("details", JSObject())!!

        val config = this.configs[runnerLabel] ?: throw Exception("no runner config found for label")

        GlobalScope.launch(Dispatchers.Default) {
            try {
                val srcFile = this@BackgroundRunnerPlugin.bridge.context.assets.open("public/${config.src}").bufferedReader().use {
                    it.readText()
                }

                val runner = Runner()
                val runnerContext = runner.createContext(config.label)
                runnerContext?.execute(srcFile, false)

                val result = MutableStateFlow<Result<JSONObject?>?>(null)

                class CompletionCallback: JSFunction(args = null) {
                    override fun run() {
                        super.run()
                        result.value = Result.success(this.args)
                    }
                }

                val callback = CompletionCallback()
                runnerContext?.registerFunction("_backgroundCallback", callback)

                details.put("completed", "__ebr::_backgroundCallback")

                runnerContext.dispatchEvent(runnerEvent, details)

                val finishedResult = result.conditionalAwait {
                    it != null
                }

                runner.destroy()

                if (finishedResult!!.isSuccess) {
                    val returnData = finishedResult!!.getOrNull()
                    if (returnData != null) {
                        call.resolve(JSObject.fromJSONObject(returnData))
                    } else {
                        call.resolve()
                    }
                }
            } catch (ex: Exception) {
                call.reject(ex.message)
            }
        }
    }

    private fun initRunnerWorkers() {
        configs.forEach {
            val data = Data.Builder()
                .putString("label", it.value.label)
                .putString("src", it.value.label)
                .putString("event", it.value.event)
                .build()

            if (!it.value.repeat) {
                val work = OneTimeWorkRequest.Builder(RunnerWorker::class.java)
                    .setInitialDelay(it.value.interval.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.value.label)
                    .build()
                WorkManager.getInstance(this.context).enqueue(work)
            } else {
                val work = PeriodicWorkRequest.Builder(RunnerWorker::class.java, it.value.label.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.value.label)
                    .build()
                WorkManager.getInstance(this.context).enqueue(work)
            }
        }
    }
}