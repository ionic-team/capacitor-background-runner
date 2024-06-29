package io.ionic.android_js_engine.capacitor_api

interface AppAPI {
    fun setBadge(value: Int)
    fun getBadge(): Int
    fun clearBadge()
    fun getState(): String
    fun getInfo(): String
}