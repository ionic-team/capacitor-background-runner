package io.ionic.backgroundrunner.plugin.api

import android.content.Context
import android.content.SharedPreferences
import io.ionic.android_js_engine.capacitor_api.KVAPI

class KV (context: android.content.Context, label: String) : KVAPI {
    private val prefs: SharedPreferences

    init {
        this.prefs = context.getSharedPreferences(label, Context.MODE_PRIVATE)
    }

    override fun set(key: String, value: String) {
        val editor: SharedPreferences.Editor = prefs.edit()
        editor.putString(key, value)
        editor.apply()
    }

    override fun get(key: String): String? {
        return prefs.getString(key, null)
    }

    override fun remove(key: String) {
        val editor: SharedPreferences.Editor = prefs.edit()
        editor.remove(key)
        editor.apply()
    }
}