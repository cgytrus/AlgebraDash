#ifdef TRACY_ENABLE

#include <Tracy.hpp>
#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#include "../../patcher.h"
#include <memory>

class $modify(CCEGLView) {
    void swapBuffers() {
        ZoneScopedN("CCEGLView::swapBuffers")
        CCEGLView::swapBuffers();
        FrameMark
    }
};

#define AD_STR(x) #x

#define GD 0
#define COCOS2D 0

#define PROFILER_HOOK(ret, type, name, argsC, argsO) \
class $modify(type) { \
    ret name##argsO { \
        ZoneScopedN(AD_STR(type::name)) \
        return type::name##argsC; \
    } \
};
#include "hooks.h"
#undef PROFILER_HOOK

#ifdef PROFILE_MEMORY

#ifdef PROFILER_MEMORY_TRACKER
std::map<uintptr_t, bool> memoryTracker;

bool trackerCheck(uintptr_t retBoundLeft, uintptr_t retBoundRight) {
    auto upperBound = memoryTracker.upper_bound(retBoundLeft);
    return upperBound == memoryTracker.end() || (upperBound->second && memoryTracker.upper_bound(retBoundRight)->second);
}
#endif

void memoryAllocated(void* memory, size_t size, const char* messageText) {
#ifdef PROFILER_MEMORY_TRACKER
    auto memoryBoundLeft = reinterpret_cast<uintptr_t>(memory);
    auto memoryBoundRight = memoryBoundLeft + size;
    if(trackerCheck(memoryBoundLeft, memoryBoundRight)) {
        memoryTracker.insert(std::pair<uintptr_t, bool>(memoryBoundLeft, true));
        memoryTracker.insert(std::pair<uintptr_t, bool>(memoryBoundRight, false));
#endif

        TracyAlloc(memory, size)
#ifdef PROFILER_MEMORY_TRACKER
    }
#ifdef MEMORY_TRACKER_MESSAGES
    else {
        TracyMessage(messageText, strlen(messageText))
    }
#endif
#endif
}

void memoryFreed(void* memory, const char* messageText) {
#ifdef PROFILER_MEMORY_TRACKER
    auto memoryCast = reinterpret_cast<uintptr_t>(memory);
    if(memoryTracker.contains(memoryCast) && memoryTracker[memoryCast]) {
        memoryTracker.erase(memoryCast);
        memoryTracker.erase(memoryTracker.upper_bound(memoryCast));
#endif

        TracyFree(memory)
#ifdef PROFILER_MEMORY_TRACKER
    }
#ifdef MEMORY_TRACKER_MESSAGES
    else {
        TracyMessage(messageText, strlen(messageText))
    }
#endif
#endif
}

void* __cdecl operatorNewHook(size_t size) {
    auto ret = operator new(size);
    memoryAllocated(ret, size, "operator_new!!");
    return ret;
}

void __cdecl operatorDeleteHook(void* memory) {
    memoryFreed(memory, "operator_delete!!");
    operator delete(memory);
}

void __cdecl operatorDeleteArrayHook(void* memory) {
    memoryFreed(memory, "operator_delete[]!!");
    operator delete[](memory);
}

void* __cdecl mallocHook(size_t size) {
    auto ret = malloc(size);
    memoryAllocated(ret, size, "malloc!!");
    return ret;
}

void* __cdecl callocHook(size_t count, size_t size) {
    auto ret = calloc(count, size);
    memoryAllocated(ret, size * count, "calloc!!");
    return ret;
}

void __cdecl freeHook(void* memory) {
    memoryFreed(memory, "free!!");
    free(memory);
}

void* __cdecl reallocHook(void* memory, size_t newSize) {
    memoryFreed(memory, "realloc!! (free)");
    auto ret = realloc(memory, newSize);
    memoryAllocated(ret, newSize, "realloc!! (alloc)");
    return ret;
}

#endif

void initProfiler() {
#ifdef PROFILE_MEMORY
#ifdef _WIN32
    auto GD = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    auto cocos2dModule = GetModuleHandle("libcocos2d.dll");
    auto COCOS2D = reinterpret_cast<uintptr_t>(cocos2dModule);

    // memory
    patch(GD + 0x2820a4, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorNewHook)));
    patch(GD + 0x2820a8, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorDeleteHook)));
    patch(GD + 0x282158, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorDeleteArrayHook)));
    patch(COCOS2D + 0x11d2e4, pointerToBytes(reinterpret_cast<uintptr_t>(&reallocHook)));
    patch(COCOS2D + 0x11d2ec, pointerToBytes(reinterpret_cast<uintptr_t>(&callocHook)));
    patch(COCOS2D + 0x11d304, pointerToBytes(reinterpret_cast<uintptr_t>(&mallocHook)));
    patch(COCOS2D + 0x11d308, pointerToBytes(reinterpret_cast<uintptr_t>(&freeHook)));
    patch(COCOS2D + 0x11d340, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorDeleteArrayHook)));
    patch(COCOS2D + 0x11d344, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorDeleteHook)));
    patch(COCOS2D + 0x11d348, pointerToBytes(reinterpret_cast<uintptr_t>(&operatorNewHook)));
#else
#pragma warning("memory profiling only works on windows atm")
#endif
#endif
}

#endif
