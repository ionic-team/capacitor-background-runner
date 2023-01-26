package io.ionic.backgroundrunner.android

import android.util.Log
import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Test
import org.junit.runner.RunWith
import java.util.logging.Logger

//class TestConsole: JSConsole() {
//    private val logger = Logger.getLogger(RunnerContext::class.java.name)
//    private val logs = HashMap<String, MutableList<String>>()
//
//    fun getInfoLogs(): List<String> {
//        return this.logs["info"]?.toList() ?:listOf<String>()
//    }
//
//    fun getLogLogs(): List<String> {
//        return this.logs["log"]?.toList() ?:listOf<String>()
//    }
//
//    fun getDebugLogs(): List<String> {
//        return this.logs["debug"]?.toList() ?:listOf<String>()
//    }
//
//    fun getWarnLogs(): List<String> {
//        return this.logs["warn"]?.toList() ?:listOf<String>()
//    }
//
//    fun getErrorLogs(): List<String> {
//        return this.logs["error"]?.toList() ?: listOf<String>()
//    }
//
//
//    override fun info(msg: String) {
//        val str = this.getStringFromJSValue(msg)
//
//        if (logs["info"] == null) {
//            this.logs["info"] = mutableListOf<String>()
//        }
//
//        logs["info"]!!.add(str)
//        this.logger.info(str)
//
//        val nativeInteger = Runner.integerFromJNI()
//        this.logger.info("Native Integer $nativeInteger")
//    }
//
//    override fun log(msg: String) {
//        val str = this.getStringFromJSValue(msg)
//
//        if (logs["log"] == null) {
//            this.logs["log"] = mutableListOf<String>()
//        }
//
//        logs["log"]!!.add(str)
//        this.logger.info(str)
//    }
//
//    override fun debug(msg: String) {
//        val str = this.getStringFromJSValue(msg)
//
//        if (logs["debug"] == null) {
//            this.logs["debug"] = mutableListOf<String>()
//        }
//
//        logs["debug"]!!.add(str)
//        this.logger.info(str)
//    }
//
//    override fun warn(msg: String) {
//        val str = this.getStringFromJSValue(msg)
//
//        if (logs["warn"] == null) {
//            this.logs["warn"] = mutableListOf<String>()
//        }
//
//        logs["warn"]!!.add(str)
//        this.logger.info(str)
//    }
//
//    override fun error(msg: String) {
//        val str = this.getStringFromJSValue(msg)
//
//        if (logs["error"] == null) {
//            this.logs["error"] = mutableListOf<String>()
//        }
//
//        logs["error"]!!.add(str)
//        this.logger.info(str)
//    }
//}


@RunWith(AndroidJUnit4::class)
class RunnerContextTests {

    @Test
    fun exampleTests() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.background")
        val result = context.execute("const test = 1 + 2;")
        assert(result == 3)
    }


//    @Test
//    fun exampleTest() {
////        val runner = Runner()
////        val context = runner.createContext("io.ionic.test")
////
////        context.execute("let test = 123;")
////        val value = context.execute("test")
////
////        val intValue = value.cast(JSNumber::class.java);
////
////        assert(intValue.int == 123)
//    }
//
//    @Test
//    fun exampleMethodTest() {
////        val runner = Runner()
////        val context = runner.createContext("io.ionic.test")
////
////        context.execute("console.error('hello world')")
//    }

//    @Test
//    fun apiConsoleTest() {
//        val logger = TestConsole()
//
//        val runner = Runner()
//        val context = runner.createContext("io.ionic.consoleTest", logger)
//
//        context.execute("console.info('test info log');")
//        context.execute("console.log('test log');")
//        context.execute("console.debug('test debug log');")
//        context.execute("console.warn('test warn log');")
//        context.execute("console.error('test error log');")
//
//        assert(logger.getInfoLogs().contains("test info log"))
//        assert(logger.getLogLogs().contains("test log"))
//        assert(logger.getDebugLogs().contains("test debug log"))
//        assert(logger.getWarnLogs().contains("test warn log"))
//        assert(logger.getErrorLogs().contains("test error log"))
//    }
//
//    @Test
//    fun apiCustomEvent() {
//        val runner = Runner()
//        val context = runner.createContext("io.ionic.customEventTest")
//
//        context.execute("const event = new CustomEvent('myEvent');")
//        val obj = context.execute("event")
//
//        assert(obj !is JSNull)
//    }


}