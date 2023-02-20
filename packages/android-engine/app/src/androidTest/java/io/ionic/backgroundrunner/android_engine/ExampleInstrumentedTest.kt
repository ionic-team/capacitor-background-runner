package io.ionic.backgroundrunner.android_engine

import android.util.Log
import android.util.Log.DEBUG
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
    fun testNullEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("undefined", true)
        assertTrue(value.isNullOrUndefined())

        value = context.execute("const test = null; test;", true);
        assertTrue(value.isNullOrUndefined())

        runner.destroy()
    }

    @Test
    fun testBoolEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("let test = (1 == 1); test;", true)
        assertTrue(value.getBoolValue()?: false)

        value = context.execute("test = (100 == 200); test;");
        assertFalse(value.getBoolValue()?: true)

        runner.destroy()
    }

    @Test
    fun testIntegerEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("1 + 2;", true)
        assertEquals(3, value.getIntValue())

        runner.destroy()
    }

    @Test
    fun testFloatEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("10.8 + 2.77;", true)
        assertEquals(13.57f, value.getFloatValue())

        runner.destroy()
    }

    @Test
    fun testStringEval() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("'hello' + ' ' + 'world';", true)
        assertEquals("hello world", value.getStringValue())

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

        class EventCallback: JSFunction(JSONObject()) {
            override fun run() {
                super.run()

                Log.d("[Context]", "Kotlin Callback called!")
            }
        }

        val callback = EventCallback()

        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        context.registerFunction("test_EventCallback", callback)

        // setting a basic event listener
        context.execute("addEventListener('myEvent', () => { test_EventCallback(); });")
        context.dispatchEvent("myEvent", JSONObject())

        // setting multiple event listeners for the same event
//        context.execute("addEventListener('myEvent', () => { console.log('alternate event listener called'); });")
//        context.dispatchEvent("myEvent", JSONObject())
//
//        // basic event listener with details
//        context.execute("addEventListener('myEventDetails', (details) => { console.log('detailed passed: ' + details.name); });")
//
//        val detailsObject = JSONObject()
//        detailsObject.put("name", "John Doe")
//
//        context.dispatchEvent("myEventDetails", detailsObject)

        runner.destroy()
    }

    @Test
    fun testAPI_Crypto() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        var value = context.execute("const array = new Uint32Array(10);  crypto.getRandomValues(array); array;", true)
        assertEquals(10, value.getJSONObject()?.length())

        value = context.execute("crypto.randomUUID();", true)
        assertEquals(36, value.getStringValue()?.length)
        runner.destroy()
    }

    @Test
    fun testAPI_SetTimeout() {
        var runner = Runner()
        val context = runner.createContext(".io.backgroundrunner.ionic")
        context.start()

        var value = context.execute("setTimeout(() => { console.log('timeout executed'); }, 2000)", true)
        assertTrue((value.getIntValue()?: 0) > 0)
        Thread.sleep(3000)

        value = context.execute("setTimeout(() => { console.log('This timeout should not be executed'); }, 4000)", true)
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

        var value = context.execute("setInterval(() => { console.log('timeout executed'); }, 2000)", true)
        assertTrue((value.getIntValue()?: 0) > 0)

        Thread.sleep(8000)
        context.execute("clearInterval(${value.getIntValue()});")

        Thread.sleep(3000)

        context.stop()
        runner.destroy()
    }

    @Test
    fun testAPI_TextEncoder() {
        var runner = Runner()
        val context = runner.createContext(".io.backgroundrunner.ionic")
        var value = context.execute("const encoder = new TextEncoder(); encoder.encode('€');", true);

        val arrayObject = value.getJSONObject()

        assertNotNull(arrayObject)

        assertEquals(226, arrayObject?.getInt("0"))
        assertEquals(130, arrayObject?.getInt("1"))
        assertEquals(172, arrayObject?.getInt("2"))

        runner.destroy()
    }

    @Test
    fun testAPI_TextDecoder() {
        var runner = Runner()
        val context = runner.createContext(".io.backgroundrunner.ionic")

        var value = context.execute("const win1251decoder = new TextDecoder(\"windows-1251\"); win1251decoder.decode(new Uint8Array([ 207, 240, 232, 226, 229, 242, 44, 32, 236, 232, 240, 33]));", true);
        assertEquals("Привет, мир!", value.getStringValue())

        value = context.execute("const decoder = new TextDecoder(); decoder.decode(new Uint8Array([240, 160, 174, 183]));", true);
        assertEquals("\uD842\uDFB7", value.getStringValue())

        runner.destroy()
    }
}