package io.ionic.android_js_engine

class Runner {
    private val contexts: HashMap<String, Context> = HashMap()
    private var ptr: Long? = null

    init {
        System.loadLibrary("android_js_engine")

        this.ptr = initRunner()
    }

    private external fun initRunner(): Long

    private external fun hasPendingJobs(ptr: Long): Boolean

    private external fun executePendingJobs(ptr: Long)

    private external fun destroyRunner(ptr: Long)

    fun createContext(
        name: String,
        capAPI: NativeCapacitorAPI? = null,
    ): Context {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        if (hasContext(name)) {
            throw EngineErrors.RunnerException("context with name $name already exists")
        }

        val context = Context(name, runnerPtr, capAPI)

        contexts[name] = context

        return context
    }

    fun hasContext(name: String): Boolean {
        return contexts.containsKey(name)
    }

    fun getContext(name: String): Context? {
        if (!hasContext(name)) {
            return null
        }

        return contexts[name]
    }

    fun destroyContext(name: String) {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        val context = contexts[name] ?: throw EngineErrors.RunnerException("could not find context named: $name")
        context.destroy()

        this.contexts.remove(name)
    }

    fun waitForJobs() {
        while (hasPendingJobs()) {
            executePendingJobs()
        }
    }

    fun hasPendingJobs(): Boolean {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")
        return hasPendingJobs(runnerPtr)
    }

    fun executePendingJobs() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")
        executePendingJobs(runnerPtr)
    }

    fun destroy() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        if (this.contexts.isNotEmpty()) {
            val allKeys = mutableListOf<String>()
            this.contexts.keys.forEach {
                allKeys.add(it)
            }

            allKeys.forEach {
                destroyContext(it)
            }

            this.contexts.clear()
        }

        destroyRunner(runnerPtr)
    }
}
