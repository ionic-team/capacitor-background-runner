package io.ionic.backgroundrunner.plugin.api

import android.app.NotificationManager
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.os.Build
import com.getcapacitor.Logger

class TimedNotificationPublisher : BroadcastReceiver() {
    companion object {
        const val NOTIFICATION_KEY = "NotificationPublisher.notification"
    }

    override fun onReceive(context: Context?, intent: Intent?) {
        val notificationManager =
            context!!.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager

        val notification: android.app.Notification?

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            notification = intent!!.getParcelableExtra(NOTIFICATION_KEY, android.app.Notification::class.java)
        } else {
            notification = getParcelableExtraLegacy(intent!!)
        }

        notification?.`when` = System.currentTimeMillis()

        val id =
            intent.getIntExtra(Notifications.notificationIntentKey, Int.MIN_VALUE)
        if (id == Int.MIN_VALUE) {
            Logger.error(Logger.tags("LN"), "No valid id supplied", null)
        }
        notificationManager.notify(id, notification)
    }

    private fun getParcelableExtraLegacy(intent: Intent): android.app.Notification? {
        return intent.getParcelableExtra(NOTIFICATION_KEY)
    }
}