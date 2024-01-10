package io.ionic.android_js_engine

import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.Job
import kotlinx.coroutines.launch

class Runner {
    private val contexts: HashMap<String, Context> = HashMap()
    private var ptr: Long? = null
    private var job: Job? = null

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

        this.contexts.remove(name)
    }

    fun start() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")
        this.job = GlobalScope.launch {
            startRunner(runnerPtr)
        }
    }

    fun stop() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")
        this.stopRunner(runnerPtr)
        if (this.job != null) {
            this.job?.cancel()
            this.job = null
        }
    }

    fun destroy() {
        val runnerPtr = this.ptr ?: throw EngineErrors.RunnerException("pointer is nil")

        this.stop()

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