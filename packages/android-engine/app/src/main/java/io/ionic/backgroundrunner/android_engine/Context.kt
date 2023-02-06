package io.ionic.backgroundrunner.android_engine

class Context constructor(name: String, runnerPtr: Long) {
    private val ptr: Long?
    private val name: String

    init {
        this.name = name
        this.ptr = Context.initContext(runnerPtr)
    }

    companion object {
        init {
            System.loadLibrary("android_engine")
        }

        external fun initContext(runnerPtr: Long): Long
        external fun destroyContext(ptr: Long)
        external fun evaluate(ptr: Long, code: String): JSValue
    }

    fun execute(code: String): JSValue {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        return Context.evaluate(this.ptr, code)
    }

    fun destroy() {
        if (this.ptr != null) {
            Context.destroyContext(this.ptr)
        }
    }
}