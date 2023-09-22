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
import io.ionic.android_js_engine.api.CapacitorAPI
import io.ionic.backgroundrunner.plugin.api.Device
import io.ionic.backgroundrunner.plugin.api.Geolocation
import io.ionic.backgroundrunner.plugin.api.KV
import io.ionic.backgroundrunner.plugin.api.Notifications
import kotlinx.coroutines.flow.MutableStateFlow
import org.json.JSONObject
import java.io.BufferedReader
import java.io.InputStreamReader
import java.lang.Exception
import java.util.concurrent.TimeUnit


class BackgroundRunner(context: android.content.Context) {
    val config: RunnerConfig?

    private var runner: Runner = Runner()

    companion object {
        @Volatile private var instance: BackgroundRunner? = null

        fun getInstance(context: android.content.Context): BackgroundRunner = instance ?: synchronized(this) {
            instance ?: BackgroundRunner(context).also { instance = it }
        }
    }

    init {
        config = loadRunnerConfig(context.assets)
        runner.start()
    }

    fun scheduleBackgroundTask(androidContext: android.content.Context) {
        config ?: throw Exception("...no runner config to schedule")

        val interval = config.interval ?: throw Exception("cannot register background task without a configured interval")

        if (!config.autoSchedule) {
            return
        }

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
                .setInitialDelay(interval.toLong(), TimeUnit.MINUTES)
                .setInputData(data)
                .addTag(config.label)
                .build()
            WorkManager.getInstance(androidContext).enqueueUniquePeriodicWork(config.label, ExistingPeriodicWorkPolicy.UPDATE, work)
        }
    }

    suspend fun execute(androidContext: android.content.Context, config: RunnerConfig, dataArgs: JSONObject = JSONObject(), callbackId: String? = null): JSONObject? {
        config ?: throw Exception("...no runner config to start")

        val context = initContext(config, androidContext, callbackId)

        val future = MutableStateFlow<Result<JSONObject?>?>(null)

        class ResolveCallback : JSFunction(jsName = "resolve") {
            override fun run() {
                super.run()
                future.value = Result.success(this.args)
            }
        }

        class RejectCallback : JSFunction(jsName = "reject") {
            override fun run() {
                super.run()
                val rejectionTitle = this.args?.optString("name", "Error") ?: "Error"
                val rejectionMessage = this.args?.optString("message") ?: ""
                val error = Exception("$rejectionTitle: $rejectionMessage")

                future.value = Result.failure(error)
            }
        }

        val resolve = ResolveCallback()
        val reject = RejectCallback()
        context.registerFunction("_resolveCallback", resolve)
        context.registerFunction("_rejectCallback", reject)

        val args = JSONObject()

        val callbacks = JSONObject();
        callbacks.put("resolve", "__cbr::_resolveCallback")
        callbacks.put("reject", "__cbr::_rejectCallback")

        args.put("callbacks", callbacks)
        args.put("dataArgs", dataArgs)

        context.dispatchEvent(config.event, args)

        val finished = future.conditionalAwait {
            it != null
        }

        destroyContext(config)

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

    private fun initContext(config: RunnerConfig, context: android.content.Context, callbackId: String?): Context {
        val srcFile = context.assets.open("public/${config.src}").bufferedReader().use {
            it.readText()
        }

        var contextName = config.label
        if (callbackId != null) {
            contextName += "-$callbackId"
        }

        val newContext  = runner.createContext(contextName)

        val api = CapacitorAPI(config.label)
        api.initNotificationsAPI(Notifications(context))
        api.initDeviceAPI(Device(context))
        api.initGeolocationAPI(Geolocation(context))
        api.initKVAPI(KV(context, config.label))

        newContext.setCapacitorAPI(api)

        newContext.execute(srcFile, false)

        return newContext
    }

    private fun destroyContext(config: RunnerConfig) {
        runner.destroyContext(config.label)
    }
}