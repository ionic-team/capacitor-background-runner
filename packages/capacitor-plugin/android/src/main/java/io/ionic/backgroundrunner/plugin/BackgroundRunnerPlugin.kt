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
import java.lang.Exception
import java.util.concurrent.TimeUnit

@CapacitorPlugin(name = "BackgroundRunner")
class BackgroundRunnerPlugin: Plugin() {
    private var impl: BackgroundRunner? = null

    override fun load() {
        super.load()
        impl = BackgroundRunner.getInstance(this.context)

        bridge.app.setStatusChangeListener {
            if (!it) {
                Log.d("Background Runner", "registering runner workers")
                impl?.scheduleBackgroundTask(this.context)
            }
        }
    }

    @PluginMethod
    fun dispatchEvent(call: PluginCall) {
        try {
            val impl = BackgroundRunner.getInstance(this.context)

            val runnerEvent = call.getString("event") ?: throw Exception("event is missing or invalid")
            val details = call.getObject("details")
            val config = impl.config ?: throw Exception("no runner config loaded")
            config.event = runnerEvent

            GlobalScope.launch(Dispatchers.Default) {
                try {
                    val returnData = impl.execute(
                        this@BackgroundRunnerPlugin.context,
                        config,
                        details
                    )

                    if (returnData != null) {
                        call.resolve(JSObject.fromJSONObject(returnData))
                    } else {
                        call.resolve()
                    }
                } catch (ex: Exception) {
                    call.reject(ex.message)
                }
            }
        } catch(ex: Exception) {
            call.reject(ex.message)
        }
    }

    @PluginMethod
    fun registerBackgroundTask(call: PluginCall) {
        call.resolve()
    }
}