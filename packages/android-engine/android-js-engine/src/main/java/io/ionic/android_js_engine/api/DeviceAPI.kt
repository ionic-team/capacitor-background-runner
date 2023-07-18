package io.ionic.android_js_engine.api

interface DeviceAPI {
    fun getBatteryStatus(): String
    fun getNetworkStatus(): String
}
