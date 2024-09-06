package io.ionic.backgroundrunner.plugin;

import android.Manifest
import android.util.Log
import com.getcapacitor.JSObject
import com.getcapacitor.Plugin
import com.getcapacitor.PluginCall
import com.getcapacitor.PluginMethod
import com.getcapacitor.annotation.CapacitorPlugin
import com.getcapacitor.annotation.Permission
import com.getcapacitor.annotation.PermissionCallback
import io.ionic.backgroundrunner.plugin.api.AppState
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking


@CapacitorPlugin(
    name = "BackgroundRunner",
    permissions = [
        Permission(
            strings = [Manifest.permission.ACCESS_COARSE_LOCATION, Manifest.permission.ACCESS_FINE_LOCATION],
            alias = BackgroundRunnerPlugin.GEOLOCATION
        ),
        Permission(
            strings = [Manifest.permission.POST_NOTIFICATIONS],
            alias = BackgroundRunnerPlugin.NOTIFICATIONS
        )
    ]
)
class BackgroundRunnerPlugin: Plugin() {
    private var impl: BackgroundRunner? = null
    private var appState: AppState = AppState.getInstance()

    companion object {
        const val GEOLOCATION = "geolocation"
        const val NOTIFICATIONS = "notifications"
    }

    override fun handleOnStart() {
        super.handleOnStart()
        appState.isActive = true
    }

    override fun handleOnPause() {
        super.handleOnPause()
        Log.d("Background Runner", "registering runner workers")
        appState.isActive = false
        impl?.scheduleBackgroundTask(this.context)
    }

    override fun handleOnStop() {
        super.handleOnStop()
        Log.d("Background Runner", "shutting down foreground runner")
        appState.isActive = false
        impl?.shutdown()
    }

    override fun handleOnResume() {
        super.handleOnResume()
        Log.d("Background Runner", "starting foreground runner")
        appState.isActive = true
        impl?.start()
    }

    override fun load() {
        super.load()
        impl = BackgroundRunner(this.context)
        impl?.start()
    }

    @PluginMethod
    override fun checkPermissions(call: PluginCall) {
        super.checkPermissions(call)
    }

    @PluginMethod
    override fun requestPermissions(call: PluginCall) {
        val apiToRequest = call.getArray("apis").toList<String>()
        super.requestPermissionForAliases(apiToRequest.toTypedArray(), call, "completePermissionsCallback")
    }

    @PermissionCallback
    fun completePermissionsCallback(call: PluginCall) {
        super.checkPermissions(call)
    }

    @PluginMethod
    fun dispatchEvent(call: PluginCall) {
        try {
            val impl = this.impl ?: throw Exception("background runner not initialized")

            val runnerEvent = call.getString("event") ?: throw Exception("event is missing or invalid")
            val details = call.getObject("details")
            val config = impl.config ?: throw Exception("no runner config loaded")

            val runningConfig = config.copy()
            runningConfig.event = runnerEvent

            runBlocking(Dispatchers.IO) {
                try {
                    val returnData = impl.execute(
                        this@BackgroundRunnerPlugin.context,
                        runningConfig,
                        details,
                        call.callbackId
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