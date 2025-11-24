package io.ionic.backgroundrunner.plugin

import android.content.res.AssetManager
import androidx.work.Constraints
import androidx.work.Data
import androidx.work.ExistingPeriodicWorkPolicy
import androidx.work.ExistingWorkPolicy
import androidx.work.NetworkType
import androidx.work.OneTimeWorkRequest
import androidx.work.PeriodicWorkRequest
import androidx.work.WorkManager
import io.ionic.android_js_engine.Context
import io.ionic.android_js_engine.NativeCapacitorAPI
import io.ionic.android_js_engine.NativeJSFunction
import io.ionic.android_js_engine.Runner
import io.ionic.backgroundrunner.plugin.api.App
import io.ionic.backgroundrunner.plugin.api.AppState
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
    private var runner: Runner?
    val config: RunnerConfig?

    init {
        config = loadRunnerConfig(context.assets)
        runner = null
    }

    fun start() {
        this.runner = Runner()
    }

    fun shutdown() {
        val runner = runner ?: return

        runner.destroy()
        this.runner = null
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

        val constraints = Constraints.Builder()
            .setRequiredNetworkType(NetworkType.CONNECTED)
            .build()

        if (!config.repeats) {
            val work = OneTimeWorkRequest.Builder(RunnerWorker::class.java)
                .setInitialDelay(interval.toLong(), TimeUnit.MINUTES)
                .setInputData(data)
                .addTag(config.label)
                .setConstraints(constraints)
                .build()
            WorkManager.getInstance(androidContext).enqueueUniqueWork(config.label, ExistingWorkPolicy.REPLACE, work)
        } else {
            val work = PeriodicWorkRequest.Builder(RunnerWorker::class.java, interval.toLong(), TimeUnit.MINUTES)
                .setInitialDelay(interval.toLong(), TimeUnit.MINUTES)
                .setInputData(data)
                .addTag(config.label)
                .setConstraints(constraints)
                .build()
            WorkManager.getInstance(androidContext).enqueueUniquePeriodicWork(config.label, ExistingPeriodicWorkPolicy.UPDATE, work)
        }
    }

    suspend fun execute(androidContext: android.content.Context, config: RunnerConfig, dataArgs: JSONObject = JSONObject(), callbackId: String? = null): JSONObject? {
        val runner = runner ?: throw Exception("runner is not started")

        val context = initContext(config, androidContext, callbackId)

        class ResolveCallback(future: MutableStateFlow<Result<JSONObject?>?>) : NativeJSFunction(jsFunctionName = "resolve") {
            var future: MutableStateFlow<Result<JSONObject?>?>

            init {
                this.future = future
            }

            override fun run() {
                super.run()
                future.value = Result.success(this.jsFunctionArgs)
            }
        }

        class RejectCallback(future: MutableStateFlow<Result<JSONObject?>?>) : NativeJSFunction(jsFunctionName = "reject") {
            var future: MutableStateFlow<Result<JSONObject?>?>

            init {
                this.future = future
            }

            override fun run() {
                super.run()
                val rejectionTitle = this.jsFunctionArgs?.optString("name", "Error") ?: "Error"
                val rejectionMessage = this.jsFunctionArgs?.optString("message") ?: ""
                val error = Exception("$rejectionTitle: $rejectionMessage")

                future.value = Result.failure(error)
            }
        }

        val future = MutableStateFlow<Result<JSONObject?>?>(null)

        val resolve = ResolveCallback(future)
        val reject = RejectCallback(future)
        context.registerFunction("_resolveCallback", resolve)
        context.registerFunction("_rejectCallback", reject)

        val args = JSONObject()

        val callbacks = JSONObject();
        callbacks.put("resolve", "__cbr::_resolveCallback")
        callbacks.put("reject", "__cbr::_rejectCallback")

        args.put("callbacks", callbacks)
        args.put("dataArgs", dataArgs)

        context.dispatchEvent(config.event, args)

        runner.waitForJobs()

        val finished = future.conditionalAwait {
            it != null
        }

        destroyContext(context)

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

            return  RunnerConfig.fromJSON(runnerConfigObj)
        }
    }

    private fun initContext(config: RunnerConfig, context: android.content.Context, callbackId: String?): Context {
        val runner = runner ?: throw Exception("runner is not started")

        val srcFile = context.assets.open("public/${config.src}").bufferedReader().use {
            it.readText()
        }

        var contextName = config.label
        if (callbackId != null) {
            contextName += "-$callbackId"
        }

        val api = NativeCapacitorAPI()
        api.initDeviceAPI(Device(context))
        api.initKVAPI(KV(context, config.label))
        api.initGeolocationAPI(Geolocation(context))
        api.initNotificationsAPI(Notifications(context))
        api.initAppAPI(App(context))

        val newContext  = runner.createContext(contextName, api)
        newContext.execute(srcFile, false)

        return newContext
    }

    private fun destroyContext(context: Context) {
        val runner = this.runner ?: throw Exception("runner is not started")

        runner.destroyContext(context.name)
    }
}