package io.ionic.backgroundrunner.android_engine

import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.ext.junit.runners.AndroidJUnit4

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*

/**
 * Instrumented test, which will execute on an Android device.
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
@RunWith(AndroidJUnit4::class)
class ExampleInstrumentedTest {
    @Test
    fun testIntegerEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const test = 1 + 2;")
        assertEquals(true, value.isUndefined)

        value = context.execute("test");
        assertEquals(true, value.isNumber)
        assertEquals(3, value.getIntValue())

        runner.destroy()
    }

    @Test
    fun testFloatEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const test = 10.8 + 2.77;")
        assertEquals(true, value.isUndefined)

        value = context.execute("test");
        assertEquals(true, value.isNumber)
        assertEquals(13.57f, value.getFloatValue())

        runner.destroy()
    }

    @Test
    fun testStringEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const test = 'hello' + ' ' + 'world';")
        assertEquals(true, value.isUndefined)

        value = context.execute("test");
        assertEquals(true, value.isString)
        assertEquals("hello world", value.getStringValue())

        runner.destroy()
    }

    @Test
    fun testBoolEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const test = true;")
        assertEquals(true, value.isUndefined)

        value = context.execute("test");
        assertEquals(true, value.isBool)
        assertEquals(true, value.getBoolValue())

        runner.destroy()
    }

    @Test
    fun testNullEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const test = null;")
        assertEquals(true, value.isUndefined)

        value = context.execute("test");
        assertEquals(true, value.isNull)

        runner.destroy()
    }

//    @Test
//    fun useAppContext() {
//        // Context of the app under test.
////        val appContext = InstrumentationRegistry.getInstrumentation().targetContext
////        assertEquals("io.ionic.backgroundrunner.android_engine", appContext.packageName)
//
//
//    }
}