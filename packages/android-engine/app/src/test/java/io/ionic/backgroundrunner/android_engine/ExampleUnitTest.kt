package io.ionic.backgroundrunner.android_engine

import org.junit.Test

import org.junit.Assert.*

/**
 * Example local unit test, which will execute on the development machine (host).
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
class ExampleUnitTest {
    @Test
    fun addition_isCorrect() {
        val runner = Runner()
        val context = runner.createContext("io.backgroundrunner.ionic")

        context.execute("const test = 1 + 2;")
        context.execute("test");

        runner.destroy()
    }
}