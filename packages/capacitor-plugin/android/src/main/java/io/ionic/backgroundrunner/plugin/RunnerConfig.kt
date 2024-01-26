package io.ionic.backgroundrunner.plugin;

import org.json.JSONObject
import java.lang.Exception

data class RunnerConfig( val label: String,
                         val src: String,
                         val autoSchedule: Boolean,
                         val repeats: Boolean,
                         var event: String,
                         val interval: Int?) {


    companion object {
        fun fromJSON(configObject: JSONObject): RunnerConfig {
            val labelStr = configObject.getString("label")
            if (labelStr.isNullOrEmpty()) {
                throw Exception("runner label is missing or invalid")
            }

            val srcStr = configObject.getString("src")
            if (srcStr.isNullOrEmpty()) {
                throw Exception("runner source file path is missing or invalid")
            }

            return RunnerConfig(
                labelStr,
                srcStr,
                configObject.optBoolean("autoStart", false),
                configObject.optBoolean("repeat"),
                configObject.optString("event"),
                configObject.optInt("interval", 0)
            )
        }
    }

}

