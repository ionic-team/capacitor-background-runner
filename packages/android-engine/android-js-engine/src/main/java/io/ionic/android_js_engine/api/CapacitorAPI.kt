package io.ionic.android_js_engine.api

class CapacitorAPI(context: android.content.Context, contextLabel: String) {
    private val kv: KV
    private val label: String

    init {
        label = contextLabel

        kv = KV(context, label)
    }
}