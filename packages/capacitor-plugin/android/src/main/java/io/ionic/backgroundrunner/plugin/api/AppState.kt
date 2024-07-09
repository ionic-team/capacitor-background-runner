package io.ionic.backgroundrunner.plugin.api

class AppState {
    public var isActive = false

    companion object {
        @Volatile
        private var instance: AppState? = null

        fun getInstance() = instance ?: synchronized(this) {
            instance ?: AppState().also { instance = it }
        }
    }
}