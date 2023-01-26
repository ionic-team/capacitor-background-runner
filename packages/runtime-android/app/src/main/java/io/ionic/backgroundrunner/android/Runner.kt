package io.ionic.backgroundrunner.android

class Runner {
//    private val engine: QuickJS = QuickJS.Builder().build()
    private val contexts: HashMap<String, RunnerContext> = HashMap()
//    private val runtime: JSRuntime = this.engine.createJSRuntime()
    private val ptr: Long

    companion object {
        init {
            System.loadLibrary("runtime-android-native")
        }

        external fun integerFromJNI(): Int
        external fun initRunner(): Long
        external fun destroyRunner(ptr: Long)
    }

    init {
        ptr = Runner.initRunner()
    }

    fun createContext(name: String): RunnerContext {
        val newContext = RunnerContext(name, this.ptr)

        this.contexts[name] = newContext

        return newContext
    }

    fun destroy() {
        Runner.destroyRunner(this.ptr)
    }
}