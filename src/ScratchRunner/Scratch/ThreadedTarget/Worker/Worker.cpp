#include "Worker.hpp"

void Worker::_workerFunc() {
    workerRunning = true;

    while(workerRunning) {
        if (workLock.try_lock()) {
            if (work.has_value()) {
                
            }

            workLock.unlock();
        }

        bool hasValue = false;

        {
            std::shared_lock lock(workLock);
            hasValue = work.has_value();

            if (hasValue) {
                work.value()();
            }
        }

        if (hasValue) {
            std::unique_lock lock(workLock);
            work = std::nullopt;
        }

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
    }
}

Worker::Worker() {
    workerThread = std::thread(std::bind(&Worker::_workerFunc, this));
}

Worker::~Worker() {
    workerRunning = false;
    if (workerThread.joinable())
        workerThread.join();
}

void Worker::assignWork(std::function<void()> _work) {
    workLock.lock();
    work = _work;
    workLock.unlock();
}

bool Worker::isCurrentlyWorking() const {
    workLock.lock();
    bool a = work.has_value();
    workLock.unlock();
    return a;
}
