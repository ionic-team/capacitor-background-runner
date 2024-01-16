package io.ionic.android_js_engine

class NativeJSFetchOptions(method: String, requestHeaders: HashMap<String, String>, body: ByteArray?) {
    public val httpMethod: String
    public val headers: HashMap<String, String>
    public var body: ByteArray? = null

    init {
        this.httpMethod = method
        this.body = body
        this.headers = requestHeaders
    }
}
