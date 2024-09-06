package io.ionic.backgroundrunner.plugin.api

import org.json.JSONObject

data class SetBadgeOptions(var count: Int, var messageTitle: String, var messageSubtitle: String) {
    constructor(jsonObject: JSONObject) : this(0, "", "") {
        this.count = jsonObject.optInt("count", 0)
        this.messageTitle = jsonObject.optString("notificationTitle", "")
        this.messageSubtitle = jsonObject.optString("notificationSubtitle", "")
    }
}