package io.ionic.backgroundrunner.plugin.api

import androidx.core.content.pm.PackageInfoCompat
import com.getcapacitor.util.InternalUtils
import io.ionic.android_js_engine.capacitor_api.AppAPI
import org.json.JSONObject

class App(context: android.content.Context): AppAPI {
    private val context: android.content.Context

    init {
        this.context = context
    }

    override fun getInfo(): String {
        val packageInfo = InternalUtils.getPackageInfo(context.packageManager, context.packageName)
        val appInfo = context.applicationInfo

        val appName = if (appInfo.labelRes == 0) appInfo.nonLocalizedLabel.toString() else context.getString(appInfo.labelRes)

        val info = JSONObject()
        info.put("name", appName)
        info.put("id", packageInfo.packageName)
        info.put("build", PackageInfoCompat.getLongVersionCode(packageInfo).toString())
        info.put("version", packageInfo.versionName)

        return info.toString()
    }

    override fun getState(): String {
        val state = JSONObject()
        state.put("isActive", false)
        return state.toString()
    }
}