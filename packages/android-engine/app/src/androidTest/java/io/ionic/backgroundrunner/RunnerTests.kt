package io.ionic.backgroundrunner

import androidx.test.ext.junit.runners.AndroidJUnit4
import io.ionic.android_js_engine.JSFunction
import io.ionic.android_js_engine.Runner
import junit.framework.TestCase
import org.junit.Test
import org.junit.runner.RunWith
import java.util.concurrent.CompletableFuture
import java.util.concurrent.TimeUnit

@RunWith(AndroidJUnit4::class)
class RunnerTests {
    @Test
    fun testRunnerCreateDestroy() {
        val runner = Runner()
        runner.start()

        runner.stop()
        runner.destroy()
    }

    @Test
    fun testContextCreateDestroy() {
        val runner = Runner()
        runner.start()

        val context = runner.createContext("io.ionic.android_js_engine")

        runner.destroy()
    }

    @Test
    fun testMultipleContexts() {
        val runner = Runner()
        runner.start()

        val count = (20..40).random()

        val timeoutFuture = CompletableFuture<Int>()

        class TimeoutCallback : JSFunction(jsName = "timeoutCallback") {
            public var calls: Int = 0
            override fun run() {
                super.run()
                calls++

                if (calls >= count) {
                    timeoutFuture.complete(calls)
                }
            }
        }

        val callback1 = TimeoutCallback()

        for (i in 1..count) {
            val context = runner.createContext("context #$i")
            context.registerFunction("timeoutCallback", callback1)
            context.execute("setTimeout(() => { timeoutCallback(); }, 2000)", false)
        }

        TestCase.assertEquals(count, timeoutFuture.get(5, TimeUnit.SECONDS))

        runner.destroy()
    }
}