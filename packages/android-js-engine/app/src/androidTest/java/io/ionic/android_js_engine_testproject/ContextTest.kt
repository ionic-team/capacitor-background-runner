package io.ionic.android_js_engine_testproject

import io.ionic.android_js_engine.Runner
import androidx.test.ext.junit.runners.AndroidJUnit4
import io.ionic.android_js_engine.NativeJSFunction
import org.json.JSONObject
import org.junit.Test
import org.junit.runner.RunWith
import org.junit.Assert.*
import java.util.concurrent.CompletableFuture
import java.util.concurrent.TimeUnit
import java.util.concurrent.TimeoutException

@RunWith(AndroidJUnit4::class)
class ContextTest {
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

        class SuccessCallback1 : NativeJSFunction(jsFunctionName = "successCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                future1.complete(calls)
            }
        }

        class SuccessCallback2: NativeJSFunction(jsFunctionName = "altSuccessCallback") {
            public var calls: Int = 0

            override fun run() {
                super.run()
                calls++

                future2.complete(calls)
            }
        }

        class SuccessCallback3 : NativeJSFunction(jsFunctionName = "successCallbackDetails") {
            public var calls: Int = 0

            override fun run() {
                super.run()
                calls++

                future3.complete(jsFunctionArgs)
            }
        }

        class SuccessCallback4 : NativeJSFunction(jsFunctionName = "successCallbackFunction") {
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

        var localizedMessage = throwingCodeEx.localizedMessage ?: ""
        assertTrue(localizedMessage.contains("JS exception"))

        val badCodeEx = assertThrows(Exception::class.java) {
            // badly formed code
            context.execute("addEventListener(")
        }

        localizedMessage = badCodeEx.localizedMessage ?: ""
        assertTrue(localizedMessage.contains("JS exception"))


        val throwingEventEx = assertThrows(Exception::class.java) {
            context.execute("addEventListener('myThrowingEvent', () => { throw new Error('this event throws an error') })")
            context.dispatchEvent("myThrowingEvent", JSONObject())
        }

        localizedMessage = throwingEventEx.localizedMessage ?: ""
        assertTrue(localizedMessage.contains("JS exception"))

        // testing handling JVM exceptions
        class ExceptionCallback : NativeJSFunction(jsFunctionName = "exceptionCallback") {
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

        localizedMessage = throwingJVMEventEx.localizedMessage ?: ""
        assertTrue(localizedMessage.contains("JS exception"))

        runner.destroy()
    }

    @Test
    fun testAPI_SetTimeout() {
        val runner = Runner()

        val context = runner.createContext("io.ionic.android_js_engine")

        val timeoutFuture1 = CompletableFuture<Int>()
        val timeoutFuture2 = CompletableFuture<Int>()

        class TimeoutCallback1 : NativeJSFunction(jsFunctionName = "timeoutCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                timeoutFuture1.complete(calls)
            }
        }

        class TimeoutCallback2 : NativeJSFunction(jsFunctionName = "cancelTimeoutCallback") {
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

        runner.waitForJobs()
        assertEquals(1, timeoutFuture1.get(3, TimeUnit.SECONDS))

        value = context.execute("setTimeout(() => { cancelTimeoutCallback() }, 4000)", true)
        timerId = value.getIntValue() ?: 0
        assertTrue(timerId > 0)

        context.execute("clearTimeout(${value.getIntValue()});")
        try {
            runner.waitForJobs()
            assertEquals(0, timeoutFuture2.get(3, TimeUnit.SECONDS))
        } catch (ex: TimeoutException) {
            assertTrue(true)
        }

        runner.destroy()
    }

    @Test
    fun testAPI_SetInterval() {
        val runner = Runner()

        val context = runner.createContext("io.ionic.android_js_engine")

        var calls = 0;

        class IntervalCallback : NativeJSFunction(jsFunctionName = "intervalCallback") {
            override fun run() {
                super.run()
                calls++
            }
        }

        val callback = IntervalCallback()

        context.registerFunction("intervalCallback", callback)

        val value = context.execute("setInterval(() => { intervalCallback() }, 2000)", true)
        assertTrue((value.getIntValue() ?: 0) > 0)

        Thread {
            Thread.sleep(9000)
            assertEquals(4, calls)

            context.execute("clearInterval(${value.getIntValue()});")

            Thread.sleep(3000)
            assertEquals(4, calls)
        }.start()

        runner.waitForJobs()
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

        val context = runner.createContext("io.ionic.android_js_engine")

        val future1 = CompletableFuture<JSONObject?>()
        val future2 = CompletableFuture<Int>()
        val future3 = CompletableFuture<JSONObject?>()
        val future4 = CompletableFuture<Int>()

        class SuccessCallback : NativeJSFunction(jsFunctionName = "successCallback") {
            override fun run() {
                super.run()
                future1.complete(this.jsFunctionArgs)
            }
        }

        class ImageSuccessCallback: NativeJSFunction(jsFunctionName = "successCallback3") {
            override fun run() {
                super.run()
                future4.complete(this.jsFunctionArgs?.getInt("size"))
            }
        }

        class FailureCallback : NativeJSFunction(jsFunctionName = "failureCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++
                future2.complete(calls)
            }
        }

        class OptionsSuccessCallback : NativeJSFunction(jsFunctionName = "successCallback2") {
            override fun run() {
                super.run()
                future3.complete(this.jsFunctionArgs)
            }
        }

        val callback1 = SuccessCallback()
        val callback2 = FailureCallback()
        val callback3 = OptionsSuccessCallback();
        val callback4 = ImageSuccessCallback();

        context.registerFunction("successCallback", callback1)
        context.registerFunction("failureCallback", callback2)
        context.registerFunction("successCallback2", callback3)
        context.registerFunction("successCallback3", callback4)

        val basicFetchExample = """
            fetch('https://jsonplaceholder.typicode.com/todos/1')
                .then(response => response.json())
                .then(json => { successCallback(json); })
                .catch(err => { console.error(err); });
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
            .then(json => { successCallback2(json); })
        """.trimIndent()

        val fetchImageExample = """
            fetch('https://placehold.co/600x400@3x.png')
            .then((res) => { return res.blob() })
            .then((blob) => { successCallback3({ size: blob.size }); });
        """.trimIndent()

        context.execute(basicFetchExample)
        runner.waitForJobs()
        val jsonResponse1 = future1.get(5, TimeUnit.SECONDS);
        assertEquals("delectus aut autem", jsonResponse1?.getString("title"))

        context.execute(fetchFailureExample)
        runner.waitForJobs()
        assertEquals(1, future2.get(5, TimeUnit.SECONDS))

        context.execute(fetchWithOptionsExample)
        runner.waitForJobs()
        val jsonResponse2 = future3.get(5, TimeUnit.SECONDS)
        assertEquals("bar", jsonResponse2?.getString("body"))

        val fetchMultiple = """
            const p1 = fetch('https://jsonplaceholder.typicode.com/posts', {
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
                .then(json => { console.log(JSON.stringify(json)); });
            
            const p2 = fetch('https://jsonplaceholder.typicode.com/todos/1')
                .catch(err => { console.error(err); })
                .then(response => response.json())
                .then(json => { console.log(JSON.stringify(json)); });
             
            Promise.all([p1, p2]).then(() => { console.log("done!"); });
        """.trimIndent()

        context.execute(fetchMultiple)
        runner.waitForJobs()

        context.execute(fetchImageExample)
        runner.waitForJobs()
        val imageBlobSize = future4.get()
        assertEquals(62404, imageBlobSize)

        runner.destroy()
    }

    @Test
    fun testAPI_Blob() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val future1 = CompletableFuture<Int>()
        val future2 = CompletableFuture<String>()

        class ArrayBufferCallback: NativeJSFunction(jsFunctionName = "arrayBufferCallback") {
            override fun run() {
                super.run()
                future1.complete(this.jsFunctionArgs?.getInt("size"))
            }
        }

        class TextCallback: NativeJSFunction(jsFunctionName = "textCallback") {
            override fun run() {
                super.run()
                future2.complete(jsFunctionArgs?.getString("str"))
            }
        }

        val callback1 = ArrayBufferCallback()
        val callback2 = TextCallback()

        context.registerFunction("arrayBufferCallback", callback1)
        context.registerFunction("textCallback", callback2)

        val basicBlobExample = """
            const testString = "testing";
            const testArrayBuf = new ArrayBuffer(32);
            const testTypedArr = new Int8Array(16);
            const blob = new Blob([testString, testArrayBuf, testTypedArr]);
        """.trimIndent()

        context.execute(basicBlobExample)

        val getBlobArrayBufferExample = "blob.arrayBuffer().then((buffer) => { arrayBufferCallback({size: buffer.byteLength}); });"
        context.execute(getBlobArrayBufferExample)
        runner.waitForJobs()

        val arrayBufferResponse = future1.get()
        assertEquals(55, arrayBufferResponse)

        val getBlobTextExample = "blob.text().then((text) => { textCallback({str: text}); });"
        context.execute(getBlobTextExample)
        runner.waitForJobs()

        val textResponse = future2.get()
        assertEquals("testing", textResponse)

        runner.destroy()
    }

    @Test
    fun testAPI_Headers() {
        val runner = Runner()
        val context = runner.createContext("io.ionic.android_js_engine")

        val basicHeadersExample = "const h = new Headers();";
        context.execute(basicHeadersExample)

        val headersSetExample = """
            h.set("Content-Type", "application/json");
        """.trimIndent()

        context.execute(headersSetExample)

        val headersGetExample = """
            h.get("Content-Type");
        """.trimIndent()

        var headerValue = context.execute(headersGetExample, true)
        assertEquals(headerValue.getStringValue(), "application/json")

        val headersHasExample = """
            h.has("Content-Type");
        """.trimIndent()

        headerValue = context.execute(headersHasExample, true)
        assertTrue(headerValue.getBoolValue() ?: false)

        val headersDeleteExample = """
            h.delete("Content-Type");
        """.trimIndent()

        context.execute(headersDeleteExample)

        val deletedValue = context.execute(headersGetExample, true)
        assertTrue(deletedValue.isNullOrUndefined())

        headerValue = context.execute(headersHasExample, true)
        assertFalse(headerValue.getBoolValue() ?: true)

        val headersAppendExample = """
            h.set("Accept", "text/html");
            h.append("Accept", "application/xhtml+xml");
        """.trimIndent()

        context.execute(headersAppendExample)

        headerValue = context.execute("h.get(\"Accept\")", true)
        assertEquals(headerValue.getStringValue(), "text/html, application/xhtml+xml")

        val headersAllKeysExample = """
            h.keys();
        """.trimIndent()

        context.execute(headersSetExample, false)
        val keysArrayValue = context.execute(headersAllKeysExample, true)

        val keys = keysArrayValue.getJSONArray() ?: throw Error("keys array is null")
        assertEquals(keys.length(), 2)

        val headersAllValuesExample = """
            h.values();
        """.trimIndent()

        val valuesArrayValue = context.execute(headersAllValuesExample, true)

        val values = valuesArrayValue.getJSONArray() ?: throw Error("values array is null")
        assertEquals(values.length(), 2)

        runner.destroy()
    }
}