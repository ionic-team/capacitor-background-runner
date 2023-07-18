#ifndef CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
#define CAPACITOR_BACKGROUND_RUNNER_RUNNER_H

#include <thread>

#include "quickjs/quickjs.h"

class Runner {
public:
    JSRuntime *rt;

    Runner();
    ~Runner();

    void start_run_loop();
    void stop_run_loop();

private:
    std::thread run_loop_thread;
    std::mutex run_loop_mutex;

    bool run_loop_stopped;
    bool end_run_loop;

    void run_loop();
};


#endif //CAPACITOR_BACKGROUND_RUNNER_RUNNER_H
