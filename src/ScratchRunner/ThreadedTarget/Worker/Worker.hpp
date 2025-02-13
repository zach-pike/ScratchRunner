#pragma once

#include <atomic>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <functional>
#include <optional>

class Worker {
private:
    std::atomic_bool workerRunning = false;
    std::thread workerThread;

    mutable std::shared_mutex workLock;
    std::optional<std::function<void()>> work;

    void _workerFunc();
public:
    Worker();
    ~Worker();

    void assignWork(std::function<void()> work);
    bool isCurrentlyWorking() const;
};
