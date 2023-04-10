package io.ionic.backgroundrunner.plugin;

import android.content.Context
import androidx.work.Data
import androidx.work.Worker
import androidx.work.WorkerParameters
import io.ionic.backgroundrunner.Runner

class RunnerWorker(context: Context, workerParams: WorkerParameters) : Worker(context, workerParams) {
    override fun doWork(): Result {
        try {
            val label = this.inputData.getString("label") ?: ""
            val src = this.inputData.getString("src") ?: ""
            val event = this.inputData.getString("event") ?: ""

            if (label.isEmpty() || src.isEmpty() || event.isEmpty()) {
                throw Exception("label is empty")
            }

            val runner = Runner()
            val runnerContext = runner.createContext(label)

            runnerContext.execute("console.log('hello world from a android background task!');", false)

            runner.destroy()
            return Result.success()
        } catch (ex: Exception) {
            val data = Data.Builder()
                .putString("error", ex.toString())
                .build()

            return Result.failure(data)
        }

    }
}