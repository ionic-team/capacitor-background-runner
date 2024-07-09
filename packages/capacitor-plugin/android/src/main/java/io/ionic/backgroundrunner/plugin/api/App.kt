package io.ionic.backgroundrunner.plugin.api

import androidx.core.content.pm.PackageInfoCompat
import com.getcapacitor.util.InternalUtils
import io.ionic.android_js_engine.capacitor_api.AppAPI
import org.json.JSONObject

class App(private val context: android.content.Context): AppAPI {

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
        state.put("isActive", AppState.getInstance().isActive)
        return state.toString()
    }
}