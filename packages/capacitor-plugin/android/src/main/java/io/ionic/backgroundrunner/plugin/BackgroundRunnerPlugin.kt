package io.ionic.backgroundrunner.plugin;

import androidx.work.Data
import androidx.work.OneTimeWorkRequest
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.PeriodicWorkRequest
import androidx.work.WorkManager
import com.getcapacitor.Plugin
import com.getcapacitor.PluginCall
import com.getcapacitor.PluginMethod
import com.getcapacitor.annotation.CapacitorPlugin
import java.util.concurrent.TimeUnit

@CapacitorPlugin(name = "BackgroundRunner")
class BackgroundRunnerPlugin: Plugin() {
    private var configs: MutableList<RunnerConfig> = mutableListOf()

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

            configs.add(runnerConfig)
        }

        this.initRunnerWorkers()
    }

    @PluginMethod
    fun echo(call: PluginCall) {
        call.unimplemented()
    }

    @PluginMethod
    fun init(call: PluginCall) {
        call.unimplemented("testing on kotlin")
    }

    private fun initRunnerWorkers() {
        configs.forEach {
            val data = Data.Builder()
                .putString("label", it.label)
                .putString("src", it.src)
                .putString("event", it.event)
                .build()

            if (!it.repeat) {
                val work = OneTimeWorkRequest.Builder(RunnerWorker::class.java)
                    .setInitialDelay(it.interval.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.label)
                    .build()
                WorkManager.getInstance(this.context).enqueue(work)
            } else {
                val work = PeriodicWorkRequest.Builder(RunnerWorker::class.java, it.interval.toLong(), TimeUnit.MINUTES)
                    .setInputData(data)
                    .addTag(it.label)
                    .build()
                WorkManager.getInstance(this.context).enqueue(work)
            }
        }
    }
}