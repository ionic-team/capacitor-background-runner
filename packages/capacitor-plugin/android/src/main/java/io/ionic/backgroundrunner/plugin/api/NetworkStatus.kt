package io.ionic.backgroundrunner.plugin.api

class NetworkStatus {
    enum class ConnectionType(val connectionType: String) {
        WIFI("wifi"), CELLULAR("cellular"), NONE("none"), UNKNOWN("unknown");
    }

    var connected = false
    var connectionType = ConnectionType.NONE
}