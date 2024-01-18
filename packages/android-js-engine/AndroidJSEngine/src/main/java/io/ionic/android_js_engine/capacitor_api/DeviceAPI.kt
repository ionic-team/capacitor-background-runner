package io.ionic.android_js_engine.capacitor_api

interface DeviceAPI {
    fun getBatteryStatus(): String

    fun getNetworkStatus(): String
}
