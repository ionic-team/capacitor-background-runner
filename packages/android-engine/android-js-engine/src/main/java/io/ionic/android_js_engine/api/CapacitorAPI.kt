package io.ionic.android_js_engine.api

class CapacitorAPI(context: android.content.Context, contextLabel: String) {
    private val label: String

    private var kv: KVAPI?
    private var device: DeviceAPI?
    private var notifications: NotificationsAPI?
    private var geolocation: GeolocationAPI?

    init {
        label = contextLabel
        notifications = null
        device = null
        kv = null
        geolocation = null
    }

    fun initNotificationsAPI(api: NotificationsAPI) {
        this.notifications = api
    }

    fun initDeviceAPI(api: DeviceAPI) {
        this.device = api
    }

    fun initGeolocationAPI(api: GeolocationAPI) {
        this.geolocation = api
    }

    fun initKVAPI(api: KVAPI) {
        this.kv = api
    }
}