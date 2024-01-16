package io.ionic.android_js_engine

import org.json.JSONArray
import org.json.JSONObject

class NativeJSValue(jsonString: String) {
    private val json: String

    init {
        this.json = jsonString
    }

    fun getIntValue(): Int? {
        return this.getJSNumber()?.toInt()
    }

    fun getFloatValue(): Float? {
        return this.getJSNumber()?.toFloat()
    }

    fun getStringValue(): String? {
        if (checkIfNullOrUndefined()) return null

        return this.json.trim('"')
    }

    fun isNullOrUndefined(): Boolean {
        return checkIfNullOrUndefined()
    }

    fun getBoolValue(): Boolean? {
        if (checkIfNullOrUndefined()) return null

        return !(this.json == "false" || this.json == "0" || this.json.isEmpty())
    }

    fun getJSONArray(): JSONArray? {
        try {
            return JSONArray(this.json)
        } catch (`_`: java.lang.Exception) {}
        return null
    }

    fun getJSONObject(): JSONObject? {
        try {
            return JSONObject(this.json)
        } catch (`_`: java.lang.Exception) {}
        return null
    }

    private fun getJSNumber(): Double? {
        if (checkIfNullOrUndefined()) return null

        return json.toDoubleOrNull()
    }

    private fun checkIfNullOrUndefined(): Boolean {
        return this.json == "undefined" || this.json == "null"
    }
}
