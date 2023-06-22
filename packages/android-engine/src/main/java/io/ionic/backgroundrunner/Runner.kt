package io.ionic.backgroundrunner

class Runner {
    private val ptr: Long?
    private val contexts: HashMap<String, Context> = HashMap()

    init {
        ptr = Runner.initRunner()
    }

    companion object {
        init {
            System.loadLibrary("android_engine")
        }

        external fun initRunner(): Long
        external fun destroyRunner(ptr: Long)
    }

    fun createContext(name: String): Context {
        if (this.ptr == null) {
            throw Exception("runner pointer is null")
        }

        val newCtx = Context(name, this.ptr)
        contexts[name] = newCtx

        return newCtx
    }

    fun destroyContext(name: String) {
        val ctx = contexts[name]
        if (ctx != null) {
            ctx.destroy()
            this.contexts.remove(name)
        }
    }

    fun destroy() {
        if (this.ptr != null) {
            if (this.contexts.isNotEmpty()) {
                this.contexts.forEach { (_, ctx) ->
                    ctx.destroy()
                }
            }
            Runner.destroyRunner(this.ptr)
        }
    }
}
