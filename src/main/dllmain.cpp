#include "../includes.h"

#include "profiler/profiler.h"

#include "loading/loading.h"
#include "rendering/rendering.h"

#define DEBUG

#ifdef DEBUG
#include <iostream>
#include <fstream>
#endif

static BS::thread_pool* _sharedPool;
BS::thread_pool* sharedPool() { return _sharedPool; }

bool _created = false;
bool (__thiscall* CCEGLView_windowShouldClose)(CCEGLView*);
bool __fastcall CCEGLView_windowShouldClose_H(CCEGLView* self) {
    auto shouldClose = CCEGLView_windowShouldClose(self);
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

void initThreadPool(HMODULE cocos2dModule) {
    MH_CreateHook(reinterpret_cast<void*>(GetProcAddress(cocos2dModule, "?windowShouldClose@CCEGLView@cocos2d@@QAE_NXZ")),
        reinterpret_cast<void*>(&CCEGLView_windowShouldClose_H),
        reinterpret_cast<void**>(&CCEGLView_windowShouldClose));
}

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
    initRenderingOptimizations(base, cocos2dModule);

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
