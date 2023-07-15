package io.ionic.backgroundrunner.plugin.api

import android.R
import android.content.Context
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat
import com.getcapacitor.Logger.config
import com.getcapacitor.plugin.util.AssetUtil
import io.ionic.android_js_engine.api.NotificationsAPI
import org.json.JSONArray


class Notifications(context: android.content.Context) : NotificationsAPI {
    private val manager: NotificationManagerCompat
    private val context: android.content.Context

    private val defaultSoundID = AssetUtil.RESOURCE_ID_ZERO_VALUE
    private var defaultSmallIconID = AssetUtil.RESOURCE_ID_ZERO_VALUE

    init {
        this.context = context
        this.manager = NotificationManagerCompat.from(context)
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
            val channelId = it.channelId ?: "default"

            val builder = NotificationCompat.Builder(context, channelId)
            builder.setContentTitle(it.title)
            builder.setContentText(it.body)
            builder.setAutoCancel(it.autoCancel)
            builder.setOngoing(it.ongoing)
            builder.setPriority(NotificationCompat.PRIORITY_DEFAULT)
            builder.setGroupSummary(it.groupSummary != null)
            builder.setSmallIcon(it.smallIcon(this.context, getDefaultSmallIcon()))

            val largeBody = it.largeBody
            if (largeBody != null) {
                val style = NotificationCompat.BigTextStyle()
                style.bigText(largeBody)
                style.setSummaryText(it.summaryText)
            }

            this.manager.notify(it.id, builder.build())
        }
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
            resId = R.drawable.ic_dialog_info
        }
        defaultSmallIconID = resId
        return resId
    }
}