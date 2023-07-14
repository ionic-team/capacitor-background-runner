package io.ionic.backgroundrunner.plugin;

import android.Manifest
import android.util.Log
import com.getcapacitor.JSArray
import com.getcapacitor.JSObject
import com.getcapacitor.Plugin
import com.getcapacitor.PluginCall
import com.getcapacitor.PluginMethod
import com.getcapacitor.annotation.CapacitorPlugin
import com.getcapacitor.annotation.Permission
import com.getcapacitor.annotation.PermissionCallback
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import io.ionic.android_js_engine.api.Geolocation
import org.json.JSONObject

@CapacitorPlugin(
    name = "BackgroundRunner",
    permissions = [
        Permission(
            strings = [Manifest.permission.ACCESS_COARSE_LOCATION],
            alias = BackgroundRunnerPlugin.GEOLOCATION
        )
    ]
)
class BackgroundRunnerPlugin: Plugin() {
    private var impl: BackgroundRunner? = null

    companion object {
        const val GEOLOCATION = "geolocation"
    }

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