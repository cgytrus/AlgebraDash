#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <filesystem>

#include <fstream>

#include <iostream>
#include <cassert>

constexpr auto PROXY_DLL = TEXT("XInput9_1_0.dll");
constexpr auto PROXY_MAX_PATH = 260;

#define DLL_PROXY_ORIGINAL(name) original_##name

#define DLL_NAME(name) \
    FARPROC DLL_PROXY_ORIGINAL(name); \
    void _##name() { \
        DLL_PROXY_ORIGINAL(name)(); \
    }
#include "../include/proxy.h"
#undef DLL_NAME

std::filesystem::path getSystemDirectory() {
    const auto systemDirectory(std::make_unique<TCHAR[]>(PROXY_MAX_PATH));
    ::GetSystemDirectory(systemDirectory.get(), PROXY_MAX_PATH);
    return {systemDirectory.get()};
}

bool loadProxy() {
    const auto libPath = getSystemDirectory() / PROXY_DLL;
    const auto lib = LoadLibraryW(libPath.c_str());
    if(!lib)
        return false;

    #define DLL_NAME(name) DLL_PROXY_ORIGINAL(name) = GetProcAddress(lib, ###name);
    #include "../include/proxy.h"
    #undef DLL_NAME

    return true;
}

bool loadMod() {
    LoadLibraryA("AlgebraDash.dll");
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if(ul_reason_for_call != DLL_PROCESS_ATTACH)
        return TRUE;

    if(!loadProxy()) {
        MessageBoxA(NULL, "Failed to load original dll", NULL, MB_OK);
        return FALSE;
    }

    if(!loadMod()) {
        MessageBoxA(NULL, "Failed to load mod", NULL, MB_OK);
        return FALSE;
    }

    return TRUE;
}