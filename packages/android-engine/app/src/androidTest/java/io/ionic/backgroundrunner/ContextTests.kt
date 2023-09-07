package io.ionic.backgroundrunner

import androidx.test.ext.junit.runners.AndroidJUnit4
import io.ionic.android_js_engine.JSFunction
import io.ionic.android_js_engine.Runner
import junit.framework.TestCase.assertTrue
import junit.framework.TestCase.assertEquals
import junit.framework.TestCase.assertFalse
import junit.framework.TestCase.assertNotNull
import org.json.JSONObject
import org.junit.Assert.assertThrows
import org.junit.Test
import org.junit.runner.RunWith
import java.util.concurrent.CompletableFuture
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException

@RunWith(AndroidJUnit4::class)
class ContextTests {
    @Test
    fun testNullEval() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        var value = context.execute("undefined", true)
        assertTrue(value.isNullOrUndefined())

        value = context.execute("const test = null; test;", true)
        assertTrue(value.isNullOrUndefined())

        runner.destroy()
    }

    @Test
    fun testBoolEval() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        var value = context.execute("let test = (1 == 1); test;", true)
        assertTrue(value.getBoolValue() ?: false)

        value = context.execute("test = (100 == 200); test;", true)
        assertFalse(value.getBoolValue() ?: true)

        runner.destroy()
    }

    @Test
    fun testIntegerEval() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val value = context.execute("1 + 2;", true)
        assertEquals(3, value.getIntValue())

        runner.destroy()
    }

    @Test
    fun testFloatEval() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val value = context.execute("10.8 + 2.77;", true)
        assertEquals(13.57f, value.getFloatValue())

        runner.destroy()
    }

    @Test
    fun testStringEval() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val value = context.execute("'hello' + ' ' + 'world';", true)
        assertEquals("hello world", value.getStringValue())

        runner.destroy()
    }

    @Test
    fun testAPI_Console() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        context.execute("console.log('hello world');")
        context.execute("console.info('this message is for informational purposes');")
        context.execute("console.warn('this is a warning message');")
        context.execute("console.error('a problem has occurred');")
        context.execute("console.debug('this is a debugging statement');")

        runner.destroy()
    }

    @Test
    fun testAPI_EventListeners() {
        val future1 = CompletableFuture<Int>()
        val future2 = CompletableFuture<Int>()
        val future3 = CompletableFuture<JSONObject?>()
        val future4 = CompletableFuture<Int>()

        class SuccessCallback1 : JSFunction(jsName = "successCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                future1.complete(calls)
            }
        }

        class SuccessCallback2: JSFunction(jsName = "altSuccessCallback") {
            public var calls: Int = 0

            override fun run() {
                super.run()
                calls++

                future2.complete(calls)
            }
        }

        class SuccessCallback3 : JSFunction(jsName = "successCallbackDetails") {
            public var calls: Int = 0

            override fun run() {
                super.run()
                calls++

                future3.complete(args)
            }
        }

        class SuccessCallback4 : JSFunction(jsName = "successCallbackFunction") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                future4.complete(calls)
            }
        }

        val callback1 = SuccessCallback1()
        val callback2 = SuccessCallback2()
        val callback3 = SuccessCallback3()
        val callback4 = SuccessCallback4()

        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        context.registerFunction("successCallback", callback1)
        context.registerFunction("altSuccessCallback", callback2)
        context.registerFunction("successCallbackDetails", callback3)
        context.registerFunction("successCallbackFunction", callback4)

        // setting a basic event listener
        context.execute("addEventListener('myEvent', () => { successCallback(); });")
        context.dispatchEvent("myEvent", JSONObject())
        assertEquals(1, future1.get())

        context.execute("addEventListener('myEvent', () => { altSuccessCallback(); });")
        context.dispatchEvent("myEvent", JSONObject())
        assertEquals(1, future2.get())

        // basic event listener with details
        context.execute("addEventListener('myEventDetails', (details) => { successCallbackDetails(details); });")

        val detailsObject = JSONObject()
        detailsObject.put("name", "John Doe")
        context.dispatchEvent("myEventDetails", detailsObject)

        val returnedDetails = future3.get()
        assertNotNull(returnedDetails)
        assertEquals("John Doe", returnedDetails?.getString("name"))

        // basic event listener with registered global function as callback arg
        val args = JSONObject()
        val callbacks = JSONObject();
        callbacks.put("resolve", "__cbr::successCallbackFunction")

        args.put("callbacks", callbacks)

        context.execute("addEventListener('myEventCallback', (resolve) => { resolve() });")
        context.dispatchEvent("myEventCallback", args)
        assertEquals(1, future4.get(5, TimeUnit.SECONDS))

        runner.destroy()
    }

    @Test
    fun testErrorHandling() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val throwingCodeEx = assertThrows(Exception::class.java) {
            context.execute("() => { throw new Error('this method has an error'); }();")
        }

        assertTrue(throwingCodeEx.localizedMessage.contains("JS exception"))

        val badCodeEx = assertThrows(Exception::class.java) {
            // badly formed code
            context.execute("addEventListener(")
        }

        assertTrue(badCodeEx.localizedMessage.contains("JS exception"))


        val throwingEventEx = assertThrows(java.lang.Exception::class.java) {
            context.execute("addEventListener('myThrowingEvent', () => { throw new Error('this event throws an error') })")
            context.dispatchEvent("myThrowingEvent", JSONObject())
        }

        assertTrue(throwingEventEx.localizedMessage.contains("JS exception"))

        // testing handling JVM exceptions
        class ExceptionCallback : JSFunction(jsName = "exceptionCallback") {
            override fun run() {
                super.run()
                throw Exception("this is a problem from the JVM")
            }
        }

        val callback = ExceptionCallback()

        context.registerFunction("exceptionCallback", callback)

        val throwingJVMEventEx = assertThrows(java.lang.Exception::class.java) {
            context.execute("addEventListener('myThrowingJVMEvent', () => { exceptionCallback(); })")
            context.dispatchEvent("myThrowingJVMEvent", JSONObject())
        }

        assertTrue(throwingJVMEventEx.localizedMessage.contains("JS exception"))

        runner.destroy()
    }

    @Test
    fun testAPI_SetTimeout() {
        val runner = Runner()
        runner.start()

        val context = runner.createContext("io.ionic.android_js_engine")

        val timeoutFuture1 = CompletableFuture<Int>()
        val timeoutFuture2 = CompletableFuture<Int>()

        class TimeoutCallback1 : JSFunction(jsName = "timeoutCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                timeoutFuture1.complete(calls)
            }
        }

        class TimeoutCallback2 : JSFunction(jsName = "cancelTimeoutCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                timeoutFuture2.complete(calls)
            }
        }

        val callback1 = TimeoutCallback1()
        val callback2 = TimeoutCallback2()

        context.registerFunction("timeoutCallback", callback1)
        context.registerFunction("cancelTimeoutCallback", callback2)

        var value = context.execute("setTimeout(() => { timeoutCallback(); }, 2000)", true)
        var timerId = value.getIntValue() ?: 0

        assertTrue(timerId > 0)
        assertEquals(1, timeoutFuture1.get(3, TimeUnit.SECONDS))

        value = context.execute("setTimeout(() => { cancelTimeoutCallback() }, 4000)", true)
        timerId = value.getIntValue() ?: 0
        assertTrue(timerId > 0)

        context.execute("clearTimeout(${value.getIntValue()});")
        try {
            assertEquals(0, timeoutFuture2.get(3, TimeUnit.SECONDS))
        } catch (ex: TimeoutException) {
            assertTrue(true)
        }

        runner.stop()
        runner.destroy()
    }

    @Test
    fun testAPI_SetInterval() {
        val runner = Runner()
        runner.start()

        val context = runner.createContext("io.ionic.android_js_engine")

        var calls = 0;

        class IntervalCallback : JSFunction(jsName = "intervalCallback") {
            override fun run() {
                super.run()
                calls++
            }
        }

        val callback = IntervalCallback()

        context.registerFunction("intervalCallback", callback)

        val value = context.execute("setInterval(() => { intervalCallback() }, 2000)", true)
        assertTrue((value.getIntValue() ?: 0) > 0)

        Thread.sleep(8000)
        assertEquals(4, calls)

        context.execute("clearInterval(${value.getIntValue()});")

        Thread.sleep(3000)
        assertEquals(4, calls)

        runner.stop()
        runner.destroy()
    }

    @Test
    fun testAPI_Crypto() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        var value = context.execute("const array = new Uint32Array(10);  crypto.getRandomValues(array); array;", true)
        assertEquals(10, value.getJSONObject()?.length())

        value = context.execute("crypto.randomUUID();", true)
        assertEquals(36, value.getStringValue()?.length)
        runner.destroy()
    }

    @Test
    fun testAPI_TextEncoder() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")
        val value = context.execute("const encoder = new TextEncoder(); encoder.encode('€');", true)

        val arrayObject = value.getJSONObject()

        assertNotNull(arrayObject)

        assertEquals(226, arrayObject?.getInt("0"))
        assertEquals(130, arrayObject?.getInt("1"))
        assertEquals(172, arrayObject?.getInt("2"))

        runner.destroy()
    }

    @Test
    fun testAPI_TextDecoder() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        var value = context.execute("const win1251decoder = new TextDecoder(\"windows-1251\"); win1251decoder.decode(new Uint8Array([ 207, 240, 232, 226, 229, 242, 44, 32, 236, 232, 240, 33]));", true)
        assertEquals("Привет, мир!", value.getStringValue())

        value = context.execute("const decoder = new TextDecoder(); decoder.decode(new Uint8Array([240, 160, 174, 183]));", true)
        assertEquals("\uD842\uDFB7", value.getStringValue())

        runner.destroy()
    }

    @Test
    fun testAPI_Fetch() {
        val runner = Runner()
        runner.start()

        val context = runner.createContext("io.ionic.android_js_engine")

        val future1 = CompletableFuture<Int>()
        val future2 = CompletableFuture<Int>()
        val future3 = CompletableFuture<Int>()

        class SuccessCallback : JSFunction(jsName = "successCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++
                print("called success callback")
                future1.complete(calls)
            }
        }

        class FailureCallback : JSFunction(jsName = "failureCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++
                future2.complete(calls)
            }
        }

        class OptionsSuccessCallback : JSFunction(jsName = "successCallback2") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++
                future3.complete(calls)
            }
        }

        val callback1 = SuccessCallback()
        val callback2 = FailureCallback()
        val callback3 = OptionsSuccessCallback();

        context.registerFunction("successCallback", callback1)
        context.registerFunction("failureCallback", callback2)
        context.registerFunction("successCallback2", callback3)

        val basicFetchExample = """
            fetch('https://jsonplaceholder.typicode.com/todos/1')
                .then(response => response.json())
                .then(json => { console.log(JSON.stringify(json)); successCallback(); })
                .catch(err => { console.error(err);  successCallback(); });
        """.trimIndent()

        val fetchFailureExample = """
            fetch('https://blablabla.fake/todos/1')
                .catch(err => { console.error(err);  failureCallback(); });
        """.trimIndent()

        val fetchWithOptionsExample = """
            fetch('https://jsonplaceholder.typicode.com/posts', {
                method: 'POST',
                body: JSON.stringify({
                    title: 'foo',
                    body: 'bar',
                    userId: 1,
                }),
                headers: {
                    'Content-type': 'application/json; charset=UTF-8',
                }
            })
            .catch(err => { console.error(err); })
            .then(response => response.json())
            .then(json => { console.log(JSON.stringify(json)); successCallback2(); })
        """.trimIndent()

        context.execute(basicFetchExample)

        assertEquals(1, future1.get(5, TimeUnit.SECONDS))

        context.execute(fetchFailureExample)

        assertEquals(1, future2.get(5, TimeUnit.SECONDS))

        context.execute(fetchWithOptionsExample)

        assertEquals(1, future3.get(5, TimeUnit.SECONDS))

        runner.stop()
        runner.destroy()

    }
}