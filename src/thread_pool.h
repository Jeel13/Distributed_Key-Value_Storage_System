#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
private:
    std::vector<std::thread> workers;

    std::queue<std::function<void()>> tasks;

    std::mutex mutex;
    std::condition_variable condition;

    bool stop;

    void workerLoop();

public:
    ThreadPool(size_t numThreads);

    void enqueue(std::function<void()> task);

    ~ThreadPool();
};

#endif