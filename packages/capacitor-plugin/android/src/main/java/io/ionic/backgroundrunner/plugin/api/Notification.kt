package io.ionic.backgroundrunner.plugin.api

import android.annotation.SuppressLint
import com.getcapacitor.plugin.util.AssetUtil
import org.json.JSONObject
import java.text.SimpleDateFormat
import java.util.Date
import java.util.TimeZone

@SuppressLint("SimpleDateFormat")
class Notification(jsonObject: JSONObject) {
    var id: Int
    var title: String
    var body: String
    var ongoing: Boolean
    var autoCancel: Boolean
    var scheduleAt: Date
    var largeBody: String? = null
    var summaryText: String? = null
    var sound: String? = null
    var smallIcon: String? = null
    var largeIcon: String? = null
    var actionTypeID: String? = null
    var extra: HashMap<String, Any>? = null
    var group: String? = null
    var groupSummary: String? = null
    var channelId: String? = null

    private val jsDateFormat = "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'";

    init {
        id = jsonObject.optInt("id", -1)
        title = jsonObject.optString("title", "")
        body = jsonObject.optString("body", "")
        ongoing = jsonObject.optBoolean("ongoing", false)
        autoCancel = jsonObject.optBoolean("autoCancel", false)

        val scheduleDateString = jsonObject.optString("scheduleAt", "")
        if (scheduleDateString.isNotEmpty()) {
            val sdf = SimpleDateFormat(jsDateFormat)
            val parsedDate = sdf.parse(scheduleDateString)

            scheduleAt = parsedDate ?: Date()
        } else {
            scheduleAt = Date()
        }
    }

    fun smallIcon(context: android.content.Context, defaultIcon: Int): Int {
        var resId = AssetUtil.RESOURCE_ID_ZERO_VALUE

        if (smallIcon != null) {
            resId = AssetUtil.getResourceID(context, smallIcon, "drawable")
        }

        if (resId == AssetUtil.RESOURCE_ID_ZERO_VALUE) {
            resId = defaultIcon
        }

        return resId
    }
}