package io.ionic.backgroundrunner.android

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
        val context = runner.createContext("io.ionic.test")

        context.execute("let test = 123;")
        val value = context.execute("test")

        assert(value == 123)

    }
}