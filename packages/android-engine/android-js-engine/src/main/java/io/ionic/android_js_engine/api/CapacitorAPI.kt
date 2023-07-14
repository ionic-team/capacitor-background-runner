package io.ionic.android_js_engine.api

class CapacitorAPI(context: android.content.Context) {
    private val kv: KV

    init {
        kv = KV(context)
    }
}