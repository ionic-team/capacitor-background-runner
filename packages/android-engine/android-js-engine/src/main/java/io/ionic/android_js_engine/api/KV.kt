package io.ionic.android_js_engine.api

import android.content.Context
import android.content.SharedPreferences

class KV (context: android.content.Context) {
    private val prefs: SharedPreferences

    init {
        this.prefs = context.getSharedPreferences("", Context.MODE_PRIVATE)
    }

    fun set(key: String, value: String) {
        val editor: SharedPreferences.Editor = prefs.edit()
        editor.putString(key, value)
        editor.apply()
    }

    fun get(key: String): String? {
        return prefs.getString(key, null)
    }

    fun remove(key: String) {
        val editor: SharedPreferences.Editor = prefs.edit()
        editor.remove(key)
        editor.apply()
    }
}
