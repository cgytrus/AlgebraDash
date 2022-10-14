#include "includes.h"
#include "ThreadPool.hpp"

static ThreadPool* _sharedPool;
ThreadPool* ThreadPool::sharedPool() { return _sharedPool; }

void ThreadPool::start() {
    ZoneScoped;
    const uint32_t threadCount = std::thread::hardware_concurrency();
    threads.resize(threadCount);
    for(uint32_t i = 0; i < threadCount; i++)
        threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
}

void ThreadPool::queueJob(const std::function<void()>& job) {
    ZoneScoped;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(job);
    }
    {
        std::unique_lock<std::mutex> lock(jobCountMutex);
        ++jobCount;
    }
    jobCountCondition.notify_all();
}

void ThreadPool::queueOneJob(const std::function<void()>& job) {
    ZoneScoped;
    ThreadPool::queueJob(job);
    mutexCondition.notify_one();
}

void ThreadPool::finishQueue() {
    ZoneScoped;
    mutexCondition.notify_all();
}

void ThreadPool::stop() {
    ZoneScoped;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shouldTerminate = true;
    }
    mutexCondition.notify_all();
    {
        std::unique_lock<std::mutex> lock(jobCountMutex);
        jobCount = 0;
    }
    jobCountCondition.notify_all();
    for(std::thread& activeThread : threads)
        activeThread.join();
    threads.clear();
}

void ThreadPool::threadLoop() {
    ZoneScoped;
    while(true) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            mutexCondition.wait(lock, [this] {
                return !jobs.empty() || shouldTerminate;
            });
            if(shouldTerminate)
                return;
            job = jobs.front();
            jobs.pop();
        }
        job();
        {
            std::unique_lock<std::mutex> lock(jobCountMutex);
            --jobCount;
        }
        jobCountCondition.notify_all();
    }
}

void ThreadPool::waitForAllJobs() {
    ZoneScoped;
    std::unique_lock<std::mutex> lock(jobCountMutex);
    jobCountCondition.wait(lock, [this] {
        return jobCount <= 0;
    });
}

size_t ThreadPool::getJobCount() {
    ZoneScoped;
    std::unique_lock<std::mutex> lock(jobCountMutex);
    return jobCount;
}

bool _created = false;
bool (__thiscall* CCEGLView_windowShouldClose)(CCEGLView*);
bool __fastcall CCEGLView_windowShouldClose_H(CCEGLView* self) {
    auto shouldClose = CCEGLView_windowShouldClose(self);
    if(_created) {
        if(shouldClose && ThreadPool::sharedPool()) {
            ThreadPool::sharedPool()->stop();
            delete _sharedPool;
        }

        return shouldClose;
    }

    if(shouldClose)
        return shouldClose;

    _sharedPool = new ThreadPool();
    ThreadPool::sharedPool()->start();
    _created = true;

    return shouldClose;
}

void initThreadPool(HMODULE cocos2dModule) {
    MH_CreateHook(reinterpret_cast<void*>(GetProcAddress(cocos2dModule, "?windowShouldClose@CCEGLView@cocos2d@@QAE_NXZ")),
        reinterpret_cast<void*>(&CCEGLView_windowShouldClose_H),
        reinterpret_cast<void**>(&CCEGLView_windowShouldClose));
}
