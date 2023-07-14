package io.ionic.android_js_engine.api

import androidx.core.app.NotificationManagerCompat
import org.json.JSONArray
import java.lang.Exception

class Notifications(context: android.content.Context) {
    private val manager: NotificationManagerCompat

    init {
        this.manager = NotificationManagerCompat.from(context)
    }

    companion object {
        fun checkPermission(): String {
            return ""
        }

        fun requestPermission() {

        }
    }

    fun schedule(notifications: JSONArray) {
        if (Notifications.checkPermission() != "granted") {
            throw Exception("Capacitor Notifications permission denied")
        }



    }
}