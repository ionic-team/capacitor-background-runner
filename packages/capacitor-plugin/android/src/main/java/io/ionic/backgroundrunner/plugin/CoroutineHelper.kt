package io.ionic.backgroundrunner.plugin

import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.first

class ConditionalAwait<T>(
    private val stateFlow: StateFlow<T>,
    private val condition: (T) -> Boolean
) {
    suspend fun await(): T {
        val nowValue = stateFlow.value
        return if (condition(nowValue)) {
            nowValue
        } else {
            stateFlow.first { condition(it) }
        }
    }
}

suspend fun <T> StateFlow<T>.conditionalAwait(condition: (T) -> Boolean): T =
    ConditionalAwait(this, condition).await()