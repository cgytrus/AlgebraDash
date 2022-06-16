#include "../includes.h"
#include "../ThreadPool.hpp"

#include "profiler/profiler.h"

#include "loading/loading.h"
#include "playLayerUpdateVisibility/playLayerUpdateVisibility.h"
#include "rendering/rendering.h"

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <fstream>
#endif

DWORD WINAPI mainThread(void* hModule) {
#ifdef DEBUG
    AllocConsole();
    std::ofstream conout("CONOUT$", std::ios::out);
    std::ifstream conin("CONIN$", std::ios::in);
    std::cout.rdbuf(conout.rdbuf());
    std::cin.rdbuf(conin.rdbuf());
#endif

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    auto cocos2dModule = GetModuleHandle("libcocos2d.dll");
    auto cocos2dBase = reinterpret_cast<uintptr_t>(cocos2dModule);

    initThreadPool(cocos2dModule);
    initLoadingOptimizations(base);
    //initPlayLayerUpdateVisibilityOptimizations(base, cocos2dBase);
    initRenderingOptimizations(cocos2dModule);

    MH_EnableHook(MH_ALL_HOOKS);

#ifdef DEBUG
    std::string input;
    std::getline(std::cin, input);

    MH_Uninitialize();
    conout.close();
    conin.close();
    FreeConsole();
    FreeLibraryAndExitThread((HMODULE)hModule, 0);
#endif

    return 0;
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if(reason == DLL_PROCESS_ATTACH) {
        MH_Initialize();
#ifdef TRACY_ENABLE
        initProfiler();
#endif
        CreateThread(nullptr, 0x100, mainThread, handle, 0, nullptr);
    }
    return TRUE;
}
