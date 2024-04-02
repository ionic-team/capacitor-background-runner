#ifndef CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
#define CAPACITOR_BACKGROUND_RUNNER_RUNNER_H

#include <string>
#include <unordered_map>
#include <queue>
#include <thread>

#include <android/log.h>

#include "./quickjs/quickjs.h"
#include "context.h"

class Runner {
public:
    JSRuntime *rt;
    std::unordered_map<std::string, Context*> contexts;

    Runner();
    ~Runner();

    void start();
    void stop();

    long create_context(std::string name, JNIEnv *env);
    void destroy_context(std::string name);

private:
    bool run_loop_started;
    bool stop_run_loop;

    void run_loop();

    void log_debug(const std::string& msg);
};

#endif //CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
