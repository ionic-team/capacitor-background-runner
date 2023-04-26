package io.ionic.backgroundrunner.plugin;

import android.util.Log
import androidx.work.Data
import androidx.work.ExistingPeriodicWorkPolicy
import androidx.work.ExistingWorkPolicy
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
import kotlinx.coroutines.launch
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

        bridge.app.setStatusChangeListener {
            if (!it) {
                Log.d("Background Runner", "registering runner workers")
                this.initRunnerWorkers()
            }
        }
    }

    @PluginMethod
    fun dispatchEvent(call: PluginCall) {
        val runnerLabel = call.getString("label") ?: throw Exception("label is missing")
        val runnerEvent = call.getString("event") ?: throw Exception("event is missing")

        val details = call.getObject("details", JSObject())!!

        val config = this.configs[runnerLabel] ?: throw Exception("no runner config found for label")

        GlobalScope.launch(Dispatchers.Default) {
            try {
                val returnData = executeRunner(config, this@BackgroundRunnerPlugin.bridge.context, details, runnerEvent)

                if (returnData != null) {
                    call.resolve(JSObject.fromJSONObject(returnData))
                } else {
                    call.resolve()
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
                .putString("src", it.value.src)
                .putString("event", it.value.event)
                .build()

            if (!it.value.repeat) {
                val work = OneTimeWorkRequest.Builder(RunnerWorker::class.java)
                    .setInitialDelay(it.value.interval.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.value.label)
                    .build()
                WorkManager.getInstance(this.context).enqueueUniqueWork(it.value.label, ExistingWorkPolicy.REPLACE, work)
            } else {
                val work = PeriodicWorkRequest.Builder(RunnerWorker::class.java, it.value.label.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.value.label)
                    .build()
                WorkManager.getInstance(this.context).enqueueUniquePeriodicWork(it.value.label, ExistingPeriodicWorkPolicy.UPDATE, work)
            }
        }
    }
}