package com.capacitorjs.plugins.nativeworker;

import android.util.Log;

public class CapacitorNativeWorker {

    public String echo(String value) {
        Log.i("Echo", value);
        return value;
    }
}
