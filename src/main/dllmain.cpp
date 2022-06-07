#include "../includes.h"

//#define DEBUG

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

    MH_Initialize();

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
    if(reason == DLL_PROCESS_ATTACH)
        CreateThread(nullptr, 0x100, mainThread, handle, 0, nullptr);
    return TRUE;
}
