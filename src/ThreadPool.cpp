#include "includes.h"
#include "ThreadPool.hpp"

static ThreadPool* _sharedPool;
ThreadPool* ThreadPool::sharedPool() { return _sharedPool; }

void ThreadPool::start() {
    ZoneScoped
    // one thread is the main thread
    const uint32_t threadCount = std::thread::hardware_concurrency() - 1;
    threads.resize(threadCount);
    for(uint32_t i = 0; i < threadCount; i++)
        threads.at(i) = std::thread(&ThreadPool::threadLoop, this);
}

void ThreadPool::queueJob(const std::function<void()>& job) {
    ZoneScoped
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        jobs.push(job);
    }
    mutexCondition.notify_one();
}

void ThreadPool::stop() {
    ZoneScoped
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        shouldTerminate = true;
    }
    mutexCondition.notify_all();
    for(std::thread& activeThread : threads)
        activeThread.join();
    threads.clear();
}

void ThreadPool::tryExecuteJob() {
    ZoneScoped
    std::function<void()> job;
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if(jobs.empty() || shouldTerminate)
            return;
        job = jobs.front();
        jobs.pop();
    }
    job();
}

void ThreadPool::threadLoop() {
    ZoneScoped
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
    }
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

void initThreadPool(uintptr_t cocos2dBase) {
    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xc4c60),
        reinterpret_cast<void*>(&CCEGLView_windowShouldClose_H),
        reinterpret_cast<void**>(&CCEGLView_windowShouldClose));
}
