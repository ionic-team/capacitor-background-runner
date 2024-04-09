package io.ionic.backgroundrunner.plugin.api

import android.content.Context
import android.location.Location
import android.location.LocationManager
import androidx.core.location.LocationManagerCompat
import io.ionic.android_js_engine.capacitor_api.GeolocationAPI
import org.json.JSONObject
import java.lang.Exception

class Geolocation(context: android.content.Context) : GeolocationAPI {
    val manager: LocationManager
    val context: android.content.Context

    init {
        this.context = context
        this.manager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
    }

    companion object {
        fun isEnabled(context: Context): Boolean {
            val manager = context.getSystemService(Context.LOCATION_SERVICE) as LocationManager
            return LocationManagerCompat.isLocationEnabled(manager);
        }
    }

    override fun getCurrentPosition(): String? {
        if (!isEnabled(this.context)) {
            throw Exception("Capacitor Geolocation location services not enabled")
        }

        var lastLocation: Location? = null

        this.manager.allProviders.forEach {
            val tempLocation = this.manager.getLastKnownLocation(it)
            if (tempLocation != null) {
                if (lastLocation == null || lastLocation!!.elapsedRealtimeNanos > tempLocation.elapsedRealtimeNanos) {
                    lastLocation = tempLocation
                }
            }
        }

        val location = lastLocation ?: return null

        val locationInfo = JSONObject();
        locationInfo.put("latitude", location.latitude)
        locationInfo.put("longitude", location.longitude)
        locationInfo.put("accuracy", location.accuracy)
        locationInfo.put("altitude", location.altitude)
        locationInfo.put("speed", location.speed)
        locationInfo.put("heading", location.bearing)

        return locationInfo.toString()
    }
}