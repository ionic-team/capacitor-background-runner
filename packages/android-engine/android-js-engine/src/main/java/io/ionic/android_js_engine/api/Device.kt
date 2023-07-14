package io.ionic.android_js_engine.api

import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import org.json.JSONObject


class Device (context: android.content.Context) {
    private val context: android.content.Context

    init {
        this.context = context
    }

    fun getBatteryStatus(): String {
        val batteryInfo = JSONObject()
        batteryInfo.put("batteryLevel", this.getBatteryLevel())
        batteryInfo.put("isCharging", this.isCharging())

        return batteryInfo.toString()
    }

    fun getNetworkStatus(): String {
        val networkInfo = JSONObject()
        networkInfo.put("connected", true)
        networkInfo.put("connectionType", "")

        return networkInfo.toString()
    }

    private fun getBatteryLevel(): Float {
        val ifilter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        val batteryStatus = context.registerReceiver(null, ifilter)
        var level = -1
        var scale = -1
        if (batteryStatus != null) {
            level = batteryStatus.getIntExtra(BatteryManager.EXTRA_LEVEL, -1)
            scale = batteryStatus.getIntExtra(BatteryManager.EXTRA_SCALE, -1)
        }
        return level / scale.toFloat()
    }

    private fun isCharging(): Boolean {
        val ifilter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        val batteryStatus = context.registerReceiver(null, ifilter)
        if (batteryStatus != null) {
            val status = batteryStatus.getIntExtra(BatteryManager.EXTRA_STATUS, -1)
            return status == BatteryManager.BATTERY_STATUS_CHARGING || status == BatteryManager.BATTERY_STATUS_FULL
        }
        return false
    }
}