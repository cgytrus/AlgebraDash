#pragma once

#include <Geode/Geode.hpp>
#include <Tracy.hpp>

using namespace geode::prelude;

#include <BS_thread_pool.hpp>
BS::thread_pool* sharedPool();

template<class T>
inline static T* getAt(void* base, uintptr_t offset) {
    return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(base) + offset);
}

template<class T>
inline static T getVftable(void* base, uintptr_t offset) {
    return *reinterpret_cast<T*>(*reinterpret_cast<uintptr_t*>(base) + offset);
}
