package io.ionic.android_js_engine.api

class CapacitorAPI(context: android.content.Context, contextLabel: String) {
    private val label: String

    private val kv: KV
    private val device: Device
    private val notifications: Notifications
    private val geolocation: Geolocation

    init {
        label = contextLabel

        kv = KV(context, label)
        device = Device(context)
        notifications = Notifications(context)
        geolocation = Geolocation(context)
    }
}