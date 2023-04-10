package io.ionic.backgroundrunner.plugin;

import android.content.Context
import androidx.work.Worker
import androidx.work.WorkerParameters

class RunnerWorker(context: Context, workerParams: WorkerParameters, config: RunnerConfig) : Worker(context, workerParams) {
    private val config: RunnerConfig

    init {
        this.config = config
    }

    override fun doWork(): Result {
        
        return Result.success()
    }
}