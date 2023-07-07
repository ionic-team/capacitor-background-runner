package io.ionic.android_js_engine

class Runner {
    private val ptr: Long?
    private val contexts: HashMap<String, Context> = HashMap()

    init {
        System.loadLibrary("android_js_engine")
        this.ptr = this.initRunner()
    }

    private external fun initRunner(): Long
    private external fun destroyRunner(ptr: Long)

    fun createContext(name: String): Context {
        val runnerPtr = this.ptr ?: throw Exception("runner pointer is null")
        val newCtx = Context(name, runnerPtr)

        contexts[name] = newCtx

        return newCtx
    }

    fun destroyContext(name: String) {
        val ctx = contexts[name] ?: return
        ctx.destroy()
        this.contexts.remove(name)
    }

    fun destroy() {
        val runnerPtr = this.ptr ?: return

        if (this.contexts.isNotEmpty()) {
            this.contexts.forEach { (_, ctx) ->
                ctx.destroy()
            }
        }

        this.destroyRunner(runnerPtr)
    }
}