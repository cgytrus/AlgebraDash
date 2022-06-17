#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    static ThreadPool* sharedPool();

    void start();
    void queueOneJob(const std::function<void()>& job);
    void queueJob(const std::function<void()>& job);
    void finishQueue();
    void stop();
    void waitForAllJobs();
    size_t getJobCount();

private:
    void threadLoop();

    std::vector<std::thread> threads;

    size_t jobCount = 0;
    std::mutex jobCountMutex;
    std::condition_variable jobCountCondition;

    std::queue<std::function<void()>> jobs;
    bool shouldTerminate = false;
    std::mutex queueMutex;
    std::condition_variable mutexCondition;
};

void initThreadPool();
void deinitThreadPool();
