package io.ionic.android_js_engine

class NativeLib {

    /**
     * A native method that is implemented by the 'android_js_engine' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'android_js_engine' library on application startup.
        init {
            System.loadLibrary("android_js_engine")
        }
    }
}
