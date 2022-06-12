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
    void queueJob(const std::function<void()>& job);
    void stop();
    void tryExecuteJob();

private:
    void threadLoop();

    bool shouldTerminate = false; // Tells threads to stop looking for jobs
    std::mutex queueMutex; // Prevents data races to the job queue
    std::condition_variable mutexCondition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

void initThreadPool(uintptr_t cocos2dBase);
