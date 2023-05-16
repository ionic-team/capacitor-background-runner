package io.ionic.backgroundrunner.plugin;

data class RunnerConfig(
    val label: String,
    val src: String,
    val event: String,
    val repeat: Boolean,
    val autoStart: Boolean,
    val interval: Int,
)
