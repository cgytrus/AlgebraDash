#pragma once

#include <Tracy.hpp>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <matdash.hpp>
#include <matdash/minhook.hpp>
// shut up cocos
#pragma warning(push, 0)
#include <cocos2d.h>
#pragma warning(pop)
#include <MinHook.h>
#include <gd.h>
#undef min
#undef max

#include <BS_thread_pool.hpp>
BS::thread_pool* sharedPool();

using std::uintptr_t;

// since everything in cocos2d starts with CC, there is no chance
// for any collision, so it's safe to do this (it also makes using cocos a lot nicer imo)
using namespace cocos2d;

template<class T>
inline static T* getAt(void* base, uintptr_t offset) {
    return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}

template<class T>
inline static T getVftable(void* base, uintptr_t offset) {
    return *reinterpret_cast<T*>(*reinterpret_cast<uintptr_t*>(base) + offset);
}

static HMODULE ccModule;

#define CC_ADDR(x) GetProcAddress(ccModule, x)
