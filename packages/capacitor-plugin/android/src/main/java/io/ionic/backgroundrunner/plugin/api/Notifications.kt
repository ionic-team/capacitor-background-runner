package io.ionic.backgroundrunner.plugin.api

import android.R.drawable
import android.app.AlarmManager
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.ContentResolver
import android.content.Context
import android.content.Intent
import android.media.AudioAttributes
import android.net.Uri
import android.os.Build
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import com.getcapacitor.Logger.config
import com.getcapacitor.plugin.util.AssetUtil
import io.ionic.android_js_engine.capacitor_api.NotificationsAPI
import org.json.JSONArray

class Notifications(context: Context) : NotificationsAPI {
    private val manager: NotificationManagerCompat
    private val context: Context

    var defaultSmallIconID = AssetUtil.RESOURCE_ID_ZERO_VALUE
    var defaultSoundID = AssetUtil.RESOURCE_ID_ZERO_VALUE

    init {
        this.context = context
        this.manager = NotificationManagerCompat.from(context)

        this.createNotificationChannel()
    }
    companion object {
        const val notificationIntentKey = "LocalNotificationId"
        const val defaultNotificationChannelID = "default"
    }

    override fun schedule(jsonString: String) {
        val notifications = mutableListOf<Notification>()

        try {
            val options = JSONArray(jsonString)
            for(i in 0 until options.length()) {
                notifications.add(i, Notification(options.getJSONObject(i)))
            }
        } catch(ex: Exception) {
            throw Exception("options must be an array: ${ex.localizedMessage}")
        }

        notifications.forEach {
            val channelId = it.channelId ?: defaultNotificationChannelID

            val builder = NotificationCompat.Builder(context, channelId)
            builder.setContentTitle(it.title)
            builder.setContentText(it.body)
            builder.setAutoCancel(it.autoCancel)
            builder.setOngoing(it.ongoing)
            builder.priority = NotificationCompat.PRIORITY_DEFAULT
            builder.setGroupSummary(it.groupSummary != null)
            builder.setSmallIcon(it.smallIcon(this.context, getDefaultSmallIcon()))

            val largeBody = it.largeBody
            if (largeBody != null) {
                val style = NotificationCompat.BigTextStyle()
                style.bigText(largeBody)
                style.setSummaryText(it.summaryText)
            }

            scheduleNotification(builder.build(), it)
        }
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val name: CharSequence = "Default"
            val description = "Default"
            val importance = NotificationManager.IMPORTANCE_DEFAULT
            val channel = NotificationChannel(defaultNotificationChannelID, name, importance)
            channel.description = description
            val audioAttributes = AudioAttributes.Builder()
                .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
                .setUsage(AudioAttributes.USAGE_ALARM)
                .build()
            val soundUri = this.getDefaultSoundUrl(context)
            if (soundUri != null) {
                channel.setSound(soundUri, audioAttributes)
            }

            val notificationManager = context.getSystemService(
                NotificationManager::class.java
            )
            notificationManager.createNotificationChannel(channel)
        }
    }

    private fun getDefaultSoundUrl(context: Context): Uri? {
        val soundId = getDefaultSound(context)
        return if (soundId != AssetUtil.RESOURCE_ID_ZERO_VALUE) {
            Uri.parse(ContentResolver.SCHEME_ANDROID_RESOURCE + "://" + context.packageName + "/" + soundId)
        } else null
    }

    private fun getDefaultSmallIcon(): Int {
        if (defaultSmallIconID != AssetUtil.RESOURCE_ID_ZERO_VALUE) return defaultSmallIconID
        var resId = AssetUtil.RESOURCE_ID_ZERO_VALUE
        var smallIconConfigResourceName = config.getString("smallIcon")
        smallIconConfigResourceName = AssetUtil.getResourceBaseName(smallIconConfigResourceName)
        if (smallIconConfigResourceName != null) {
            resId = AssetUtil.getResourceID(context, smallIconConfigResourceName, "drawable")
        }
        if (resId == AssetUtil.RESOURCE_ID_ZERO_VALUE) {
            resId = drawable.ic_dialog_info
        }
        defaultSmallIconID = resId
        return resId
    }

    private fun getDefaultSound(context: Context): Int {
        if (defaultSoundID != AssetUtil.RESOURCE_ID_ZERO_VALUE) return defaultSoundID
        var resId = AssetUtil.RESOURCE_ID_ZERO_VALUE
        var soundConfigResourceName = config.getString("sound")
        soundConfigResourceName = AssetUtil.getResourceBaseName(soundConfigResourceName)
        if (soundConfigResourceName != null) {
            resId = AssetUtil.getResourceID(context, soundConfigResourceName, "raw")
        }
        defaultSoundID = resId
        return resId
    }

    private fun scheduleNotification(notification: android.app.Notification, request: Notification) {
        val alarmManager = context.getSystemService(Context.ALARM_SERVICE) as AlarmManager
        val intent = Intent(context, TimedNotificationPublisher::class.java)
        intent.putExtra(notificationIntentKey, request.id)
        intent.putExtra(TimedNotificationPublisher.NOTIFICATION_KEY, notification)
        var flags = PendingIntent.FLAG_CANCEL_CURRENT
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            flags = flags or PendingIntent.FLAG_MUTABLE
        }

        val pendingIntent =
            PendingIntent.getBroadcast(context, request.id, intent, flags)

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S && !alarmManager.canScheduleExactAlarms()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                alarmManager.setAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, request.scheduleAt.time, pendingIntent);
            } else {
                alarmManager.set(AlarmManager.RTC, request.scheduleAt.time, pendingIntent);
            }
        } else {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
                alarmManager.setExactAndAllowWhileIdle(AlarmManager.RTC_WAKEUP, request.scheduleAt.time, pendingIntent);
            } else {
                alarmManager.setExact(AlarmManager.RTC, request.scheduleAt.time, pendingIntent);
            }
        }
    }
}