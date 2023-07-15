package io.ionic.android_js_engine.api

class CapacitorAPI(context: android.content.Context, contextLabel: String) {
    private val label: String

    private val kv: KV
    private val device: Device
    private var notifications: NotificationsAPI?
    private val geolocation: Geolocation

    init {
        label = contextLabel
        notifications = null

        kv = KV(context, label)
        device = Device(context)
        geolocation = Geolocation(context)
    }

    fun initNotificationsAPI(api: NotificationsAPI) {
        this.notifications = api
    }
}