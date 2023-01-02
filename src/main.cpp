#include <includes.hpp>
#include <config.hpp>

#include <matdash/boilerplate.hpp>

#include <profiler/profiler.hpp>

#include <optimizations/base.hpp>
#include <optimizations/loading.hpp>
#include <optimizations/rendering.hpp>

#ifdef CUSTOM_DEBUG
#include <matdash/console.hpp>
#include <iostream>
#include <fstream>
#endif

static BS::thread_pool* _sharedPool;
BS::thread_pool* sharedPool() { return _sharedPool; }

namespace ad {
    bool _created = false;
    matdash::cc::thiscall<bool> CCEGLView_windowShouldClose(CCEGLView* self) {
        auto shouldClose = matdash::orig<&CCEGLView_windowShouldClose>(self);
        if(_created) {
            if(shouldClose && _sharedPool)
                delete _sharedPool;
            return shouldClose;
        }

        if(shouldClose)
            return shouldClose;

        _sharedPool = new BS::thread_pool(std::thread::hardware_concurrency() - 1);
        _created = true;

        return shouldClose;
    }

    void initThreadPool() {
        matdash::add_hook<&CCEGLView_windowShouldClose>(CC_ADDR("?windowShouldClose@CCEGLView@cocos2d@@QAE_NXZ"));
    }

    void addHooks() {
#ifdef TRACY_ENABLE
        initProfiler();
#endif

#ifdef OPTIMIZATIONS
        ad::initThreadPool();
        for(auto opt : optimizations) {
            opt->init();
        }
#endif
    }
}

void mod_main(HMODULE hModule) {
#ifdef CUSTOM_DEBUG
    auto console = new matdash::Console();
#endif

    static ad::LoadingOptimization loading;
    static ad::RenderingOptimization rendering;

    ad::optimizations.push_back(&loading);
    ad::optimizations.push_back(&rendering);

    ad::config::loadConfig();
    ad::config::initMegahack();
    if(ad::config::enabled)
        ad::config::enableHooks();

#ifdef CUSTOM_DEBUG
    std::string input;
    std::getline(std::cin, input);

    MH_Uninitialize();
    delete console;
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
#endif
}
