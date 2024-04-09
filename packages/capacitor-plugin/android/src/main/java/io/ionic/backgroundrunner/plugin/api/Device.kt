package io.ionic.backgroundrunner.plugin.api

import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.os.BatteryManager
import android.os.Build
import io.ionic.android_js_engine.capacitor_api.DeviceAPI
import org.json.JSONObject

class Device (context: android.content.Context) : DeviceAPI {
    private val context: android.content.Context
    private val connectivityManager: ConnectivityManager

    init {
        this.context = context
        this.connectivityManager = this.context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager;
    }

    override fun getBatteryStatus(): String {
        val batteryInfo = JSONObject()
        batteryInfo.put("batteryLevel", this.getBatteryLevel())
        batteryInfo.put("isCharging", this.isCharging())

        return batteryInfo.toString()
    }

    override fun getNetworkStatus(): String {
        var status = NetworkStatus()

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            val activeNetwork: Network? = this.connectivityManager.activeNetwork
            val capabilities: NetworkCapabilities? =
                this.connectivityManager.getNetworkCapabilities(this.connectivityManager.activeNetwork)
            if (activeNetwork != null && capabilities != null) {
                status.connected =
                    capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED) &&
                            capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
                if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI)) {
                    status.connectionType = NetworkStatus.ConnectionType.WIFI
                } else if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR)) {
                    status.connectionType = NetworkStatus.ConnectionType.CELLULAR
                } else {
                    status.connectionType = NetworkStatus.ConnectionType.UNKNOWN
                }
            }
        } else {
            status = getAndParseNetworkInfo()
        }

        val networkInfo = JSONObject()
        networkInfo.put("connected", status.connected)
        networkInfo.put("connectionType", status.connectionType)

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

    @SuppressWarnings("deprecation")
    private fun getAndParseNetworkInfo(): NetworkStatus {
        val networkStatus = NetworkStatus()
        val networkInfo = connectivityManager.activeNetworkInfo
        if (networkInfo != null) {
            networkStatus.connected = networkInfo.isConnected
            val typeName = networkInfo.typeName
            if (typeName == "WIFI") {
                networkStatus.connectionType = NetworkStatus.ConnectionType.WIFI
            } else if (typeName == "MOBILE") {
                networkStatus.connectionType = NetworkStatus.ConnectionType.CELLULAR
            }
        }
        return networkStatus
    }
}