package io.ionic.android_js_engine

class NativeJSResponse(statusCode: Int, url: String, data: ByteArray?, error: String?) {
    public var ok: Boolean
    public var status: Int
    public var url: String
    public var error: String?

    private var data: ByteArray?

    init {
        this.status = statusCode
        this.ok = statusCode in 200..299
        this.url = url
        this.data = data
        this.error = error
    }
}