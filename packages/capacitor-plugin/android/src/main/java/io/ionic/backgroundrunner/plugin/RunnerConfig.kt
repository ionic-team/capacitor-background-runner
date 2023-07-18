package io.ionic.backgroundrunner.plugin;

import org.json.JSONObject
import java.lang.Exception

class RunnerConfig(configObject: JSONObject) {
    val label: String
    val src: String
    val autoSchedule: Boolean
    val repeats: Boolean
    var event: String
    val interval: Int?

    init {
        val labelStr = configObject.getString("label")
        if (labelStr.isNullOrEmpty()) {
            throw Exception("runner label is missing or invalid")
        }

        val srcStr = configObject.getString("src")
        if (srcStr.isNullOrEmpty()) {
            throw Exception("runner source file path is missing or invalid")
        }

        label = labelStr
        src = srcStr
        autoSchedule = configObject.optBoolean("autoStart", false)
        event = configObject.optString("event")
        repeats = configObject.optBoolean("repeat")
        interval = configObject.optInt("interval", 0)
    }
}

