#include "../includes.h"

#include <matdash/boilerplate.hpp>

#include "profiler/profiler.h"

#include "loading/loading.h"
#include "rendering/rendering.h"

#ifdef CUSTOM_DEBUG
#include <matdash/console.hpp>
#include <iostream>
#include <fstream>
#endif

static BS::thread_pool* _sharedPool;
BS::thread_pool* sharedPool() { return _sharedPool; }

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

void mod_main(HMODULE hModule) {
    ccModule = GetModuleHandle("libcocos2d.dll");

#ifdef TRACY_ENABLE
    initProfiler();
#endif

#ifdef CUSTOM_DEBUG
    auto console = new matdash::Console();
#endif

#ifdef OPTIMIZATIONS
    initThreadPool();
    initLoadingOptimizations();
    initRenderingOptimizations();
#endif

#ifdef CUSTOM_DEBUG
    std::string input;
    std::getline(std::cin, input);

    MH_Uninitialize();
    delete console;
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
#endif
}
