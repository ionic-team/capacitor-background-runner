package io.ionic.backgroundrunner.plugin

import android.content.res.AssetManager
import androidx.work.Data
import androidx.work.ExistingPeriodicWorkPolicy
import androidx.work.ExistingWorkPolicy
import androidx.work.OneTimeWorkRequest
import androidx.work.PeriodicWorkRequest
import androidx.work.WorkManager
import io.ionic.android_js_engine.Context
import io.ionic.android_js_engine.JSFunction
import io.ionic.android_js_engine.Runner
import kotlinx.coroutines.flow.MutableStateFlow
import org.json.JSONObject
import java.io.BufferedReader
import java.io.InputStreamReader
import java.lang.Exception
import java.util.concurrent.CompletableFuture
import java.util.concurrent.TimeUnit


class BackgroundRunner(context: android.content.Context) {
    public val config: RunnerConfig?
    private var runner: Runner? = null
    private var context: Context? = null

    companion object {
        @Volatile private var instance: BackgroundRunner? = null

        fun getInstance(context: android.content.Context): BackgroundRunner = instance ?: synchronized(this) {
            instance ?: BackgroundRunner(context).also { instance = it }
        }
    }

    private val started: Boolean
        get() {
            return runner != null && context != null
        }

    init {
        config = loadRunnerConfig(context.assets)
    }

    fun start(androidContext: android.content.Context) {
        config ?: throw Exception("...no runner config to start")

        runner = Runner()
        context = initContext(config, androidContext)
    }

    fun scheduleBackgroundTask(androidContext: android.content.Context) {
        config ?: throw Exception("...no runner config to schedule")

        val interval = config.interval ?: throw Exception("cannot register background task without a configured interval")

        val data = Data.Builder()
            .putString("label", config.label)
            .putString("src", config.src)
            .putString("event", config.event)
            .build()

        if (!config.repeats) {
            val work = OneTimeWorkRequest.Builder(RunnerWorker::class.java)
                .setInitialDelay(interval.toLong(), TimeUnit.MINUTES)
                .setInputData(data)
                .addTag(config.label)
                .build()
            WorkManager.getInstance(androidContext).enqueueUniqueWork(config.label, ExistingWorkPolicy.REPLACE, work)
        } else {
            val work = PeriodicWorkRequest.Builder(RunnerWorker::class.java, interval.toLong(), TimeUnit.MINUTES)
                .setInputData(data)
                .addTag(config.label)
                .build()
            WorkManager.getInstance(androidContext).enqueueUniquePeriodicWork(config.label, ExistingPeriodicWorkPolicy.UPDATE, work)
        }
    }

    suspend fun execute(androidContext: android.content.Context, config: RunnerConfig, args: JSONObject = JSONObject()): JSONObject? {
        if (!started) {
            start(androidContext)
        }

        context ?: throw Exception("no loaded context for config")

        val future = MutableStateFlow<Result<JSONObject?>?>(null)

        class CompletionCallback : JSFunction(jsName = "completed") {
            override fun run() {
                super.run()
                future.value = Result.success(this.args)
            }
        }

        val callback = CompletionCallback()
        context!!.registerFunction("_backgroundCallback", callback)

        args.put("completed", "__cbr::_backgroundCallback")

        context!!.dispatchEvent(config.event, args)

        val finished = future.conditionalAwait {
            it != null
        }

        runner?.destroy()

        runner = null

        if (finished!!.isSuccess) {
            return finished.getOrNull()
        } else {
            val ex = finished.exceptionOrNull()
            ex ?: throw Exception("unknown exception")
            throw ex
        }
    }

    private fun loadRunnerConfig(assetManager: AssetManager): RunnerConfig {
        BufferedReader(InputStreamReader(assetManager.open("capacitor.config.json"))).use { reader ->
            val buffer = StringBuilder()
            var line: String?
            while (reader.readLine().also { line = it } != null) {
                buffer.append(line).append("\n")
            }
             val fileContents = buffer.toString();

            val configObject = JSONObject(fileContents)
            val plugins = configObject.getJSONObject("plugins") ?: throw Exception("could not read config file")
            val runnerConfigObj = plugins.getJSONObject("BackgroundRunner") ?: throw Exception("could not read config file")

            return  RunnerConfig(runnerConfigObj)
        }
    }

    private fun initContext(config: RunnerConfig, context: android.content.Context): Context {
        runner ?: throw Exception("runner is not initialized")

        val srcFile = context.assets.open("public/${config.src}").bufferedReader().use {
            it.readText()
        }

        val newContext  = runner!!.createContext(config.label)

        // TODO: Setup capacitor api

        newContext.execute(srcFile, false)

        return newContext
    }
}