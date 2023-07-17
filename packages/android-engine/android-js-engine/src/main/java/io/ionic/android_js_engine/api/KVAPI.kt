package io.ionic.android_js_engine.api

interface KVAPI {
    fun set(key: String, value: String)
    fun get(key: String): String?
    fun remove(key: String)
}
