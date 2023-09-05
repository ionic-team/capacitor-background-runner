package io.ionic.backgroundrunner

import androidx.test.ext.junit.runners.AndroidJUnit4
import io.ionic.android_js_engine.Runner
import org.junit.Test
import org.junit.runner.RunWith

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
}