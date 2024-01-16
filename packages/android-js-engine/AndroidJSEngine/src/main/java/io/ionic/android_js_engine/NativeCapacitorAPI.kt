package io.ionic.android_js_engine

import io.ionic.android_js_engine.capacitor_api.DeviceAPI
import io.ionic.android_js_engine.capacitor_api.GeolocationAPI
import io.ionic.android_js_engine.capacitor_api.KVAPI
import io.ionic.android_js_engine.capacitor_api.NotificationsAPI

class NativeCapacitorAPI {
    private var kv: KVAPI? = null
    private var device: DeviceAPI? = null
    private var notifications: NotificationsAPI? = null
    private var geolocation: GeolocationAPI? =  null

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