package io.ionic.android_js_engine.capacitor_api

interface NotificationsAPI {
    fun schedule(jsonString: String)
    fun setBadge(value: Int)
    fun clearBadge()
}
