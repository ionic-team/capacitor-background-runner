package io.ionic.android_js_engine

class Runner {
    private val contexts: HashMap<String, Context> = HashMap()

    private var ptr: Long? = null

    init {
        System.loadLibrary("android_js_engine")

        this.ptr = initRunner()
    }

    private external fun initRunner(): Long
    private external fun startRunner(ptr: Long)
    private external fun stopRunner(ptr: Long)
    private external fun destroyRunner(ptr: Long)

    fun createContext(name: String): Context {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        if (contexts.containsKey(name)) {
            throw EngineErrors.RunnerException("context with name $name already exists")
        }

        val context = Context(name, runnerPtr)

        contexts[name] = context

        return context
    }

    fun destroyContext(name: String) {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        val context = contexts[name] ?: return
        context.destroy()
        contexts.remove(name)
    }

    fun start() {}

    fun stop() {}

    fun destroy() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        this.stop()

        if (this.contexts.isNotEmpty()) {
            this.contexts.forEach { (name) ->
                destroyContext(name)
            }

            this.contexts.clear()
        }

        destroyRunner(runnerPtr)
    }
}