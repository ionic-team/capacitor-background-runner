package io.ionic.backgroundrunner.android_engine

import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.ext.junit.runners.AndroidJUnit4
import kotlinx.coroutines.delay
import org.json.JSONObject

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*
import java.util.UUID

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

    @Test
    fun testAPI_Console() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        context.execute("console.log('hello world');")
        context.execute("console.info('this message is for informational purposes');")
        context.execute("console.warn('this is a warning message');")
        context.execute("console.error('a problem has occurred');")
        context.execute("console.debug('this is a debugging statement');")

        runner.destroy()
    }

    @Test
    fun testAPI_EventListeners() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        // setting a basic event listener
        var value = context.execute("addEventListener('myEvent', () => { console.log('event listener called'); });")
        assertTrue(value.isUndefined)

        context.dispatchEvent("myEvent", JSONObject())

        // setting multiple event listeners for the same event
        value = context.execute("addEventListener('myEvent', () => { console.log('alternate event listener called'); });")
        assertTrue(value.isUndefined)

        context.dispatchEvent("myEvent", JSONObject())

        // basic event listener with details
        value = context.execute("addEventListener('myEventDetails', (details) => { console.log('detailed passed: ' + details.name); });")
        assertTrue(value.isUndefined)

        val detailsObject = JSONObject()
        detailsObject.put("name", "John Doe")

        context.dispatchEvent("myEventDetails", detailsObject)

        runner.destroy()
    }

    @Test
    fun testAPI_Crypto() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const array = new Uint32Array(10);  crypto.getRandomValues(array); for (const num of array) { console.log(num); } ")

        value = context.execute("crypto.randomUUID();")
        assertFalse(value.isUndefined)

        assertEquals(36, value.getStringValue()?.length)
        runner.destroy()
    }

    @Test
    fun testAPI_SetTimeout() {
        var runner = Runner()
        val context = runner.createContext(".io.backgroundrunner.ionic")
        context.start()

        var value = context.execute("setTimeout(() => { console.log('timeout executed'); }, 2000)")
        assertTrue((value.getIntValue()?: 0) > 0)
        Thread.sleep(3000)

        value = context.execute("setTimeout(() => { console.log('This timeout should not be executed'); }, 4000)")
        Thread.sleep(1000)

        context.execute("clearTimeout(${value.getIntValue()});")
        Thread.sleep(3000)

        context.stop()
        runner.destroy()
    }

    @Test
    fun testAPI_SetInterval() {
        var runner = Runner()
        val context = runner.createContext(".io.backgroundrunner.ionic")
        context.start()

        var value = context.execute("setInterval(() => { console.log('timeout executed'); }, 2000)")
        assertTrue((value.getIntValue()?: 0) > 0)

        Thread.sleep(8000)
        context.execute("clearInterval(${value.getIntValue()});")

        Thread.sleep(3000)

        context.stop()
        runner.destroy()
    }

    @Test
    fun testJSON() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        val json = "{\"name\":\"John Doe\",\"age\":55,\"active\":true,\"address\":{\"street\":\"123 Main Street\",\"state\":\"SD\"},\"arr\":[\"hello\",\"world\",\"test\"]}"

        var value = context.execute("JSON.parse('$json');")


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