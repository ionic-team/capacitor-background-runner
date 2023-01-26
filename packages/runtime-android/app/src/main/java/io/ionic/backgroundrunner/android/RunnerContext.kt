package io.ionic.backgroundrunner.android

import android.util.Log

class RunnerContext constructor(name: String, runnerPtr: Long) {
    private val name: String
    private val ptr: Long

    companion object {
        init {
            System.loadLibrary("runtime-android-native")
        }

        external fun initContext(runnerPtr: Long): Long
        external fun execute(ptr: Long, code: String): Int
        external fun destroyContext(ptr: Long)
    }

    init {
        this.name = name
        this.ptr = runnerPtr
        Log.d("[INIT]", this.ptr.toString())
    }

    fun execute(code: String): Int {
        val value = RunnerContext.execute(this.ptr, code)
        Log.d("[EXECUTE RESPONSE]", value.toString())
        return value
    }
}

//import android.util.Log
//import java.lang.reflect.Array
//import com.hippo.quickjs.android.JSContext
//import com.hippo.quickjs.android.JSObject
//import com.hippo.quickjs.android.JSString
//import com.hippo.quickjs.android.JSValue
//import com.hippo.quickjs.android.Method
//import com.hippo.quickjs.android.QuickJS
//
//
//class RunnerContext constructor(name: String, context: JSContext, consoleImpl: JSConsole) {
//    private val name: String
//    private val context: JSContext
//    private val console: JSConsole
//
//    init {
//        this.name = name
//        this.context = context
//        this.console = consoleImpl
//
//        this.setGlobals()
//    }
//
//    private fun apiConsole() {
//        val consoleInfoFunc = context.createJSFunction(this.console, Method.create(JSConsole::class.java, JSConsole::class.java.getMethod("info", JSValue::class.java)))
//        val consoleLogFunc = context.createJSFunction(this.console, Method.create(JSConsole::class.java, JSConsole::class.java.getMethod("log", JSValue::class.java)))
//        val consoleWarnFunc = context.createJSFunction(this.console, Method.create(JSConsole::class.java, JSConsole::class.java.getMethod("warn", JSValue::class.java)))
//        val consoleErrorFunc = context.createJSFunction(this.console, Method.create(JSConsole::class.java, JSConsole::class.java.getMethod("error", JSValue::class.java)))
//        val consoleDebugFunc = context.createJSFunction(this.console, Method.create(JSConsole::class.java, JSConsole::class.java.getMethod("debug", JSValue::class.java)))
//
//        val console = context.createJSObject()
//        console.setProperty("info", consoleInfoFunc)
//        console.setProperty("log", consoleLogFunc)
//        console.setProperty("warn", consoleWarnFunc)
//        console.setProperty("error", consoleErrorFunc)
//        console.setProperty("debug", consoleDebugFunc)
//
//        this.context.globalObject.setProperty("console", console)
//    }
//
//    private fun setGlobals() {
//        this.apiConsole()
//
//
////
////        val consoleLogDebugFunc = context.createJSFunction(this, Method.create(RunnerContext::class.java, RunnerContext::class.java.getMethod("consoleLogDebug", String::class.java)))
////        val consoleLogWarnFunc = context.createJSFunction(this, Method.create(RunnerContext::class.java, RunnerContext::class.java.getMethod("consoleLogWarn", String::class.java)))
////        val consoleLogErrorFunc = context.createJSFunction(this, Method.create(RunnerContext::class.java, RunnerContext::class.java.getMethod("consoleLogError", String::class.java)))
////
////        val console = context.createJSObject()
////        console.setProperty("info", consoleLogInfoFunc)
////        console.setProperty("debug", consoleLogDebugFunc)
////        console.setProperty("warn", consoleLogWarnFunc)
////        console.setProperty("error", consoleLogErrorFunc)
//
//    }
//
//    fun execute(code: String): JSValue {
//        try {
//            val result = this.context.evaluate(code, "", JSValue::class.java)
//            print(result)
//            return result
//        } catch (ex: Exception) {
//            print(ex.localizedMessage)
//            print(ex.stackTraceToString())
//
//            throw  ex
//        }
//    }
//
//    fun consoleLogInfo(msg: String) {
//        Log.i("INFO", msg)
//    }
//
//    fun consoleLogDebug(msg: String) {
//        Log.d("DEBUG", msg)
//    }
//
//    fun consoleLogWarn(msg: String) {
//       Log.w("WARN", msg)
//    }
//
//    fun consoleLogError(msg: String) {
//        Log.e("ERROR", msg)
//    }
//}