#ifdef TRACY_ENABLE

#include "../../includes.h"
#include "../../patcher.h"
#include <memory>

void initTracyHooks();
matdash::cc::thiscall<int> CCApplication_setupVerticalSync(CCApplication* self) {
    initTracyHooks();
    return matdash::orig<&CCApplication_setupVerticalSync>(self);
}

matdash::cc::thiscall<bool> CCEGLView_swapBuffers(CCEGLView* self) {
    ZoneScopedN("CCEGLView::swapBuffers");
    auto ret = matdash::orig<&CCEGLView_swapBuffers>(self);
    FrameMark;
    return ret;
}

#define GD gd::base
#define COCOS2D reinterpret_cast<uintptr_t>(ccModule)
#define COCOS2DSYM(name) CC_ADDR(name)

#define PROFILER_HOOK(address, name, ret, callConv, args, argsC) \
matdash::cc::callConv<ret> TracyConcat(profilerHook_, __LINE__)##args { \
    ZoneScopedN(#name); \
    return matdash::orig<&TracyConcat(profilerHook_, __LINE__)>##argsC; \
} \
inline void TracyConcat(initProfilerHook_, __LINE__)() { \
    matdash::add_hook<&TracyConcat(profilerHook_, __LINE__)>(address); \
}
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

        TracyAlloc(memory, size);
#ifdef PROFILER_MEMORY_TRACKER
    }
#ifdef MEMORY_TRACKER_MESSAGES
    else {
        TracyMessage(messageText, strlen(messageText));
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

        TracyFree(memory);
#ifdef PROFILER_MEMORY_TRACKER
    }
#ifdef MEMORY_TRACKER_MESSAGES
    else {
        TracyMessage(messageText, strlen(messageText));
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

#define PROFILER_HOOK(address, name, ret, callConv, args, argsC) TracyConcat(initProfilerHook_, __LINE__)();
void initTracyHooks() {
    // frame
    matdash::add_hook<&CCEGLView_swapBuffers>(CC_ADDR("?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"));

    #include "hooks.h"

#ifdef PROFILE_MEMORY
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
#endif
}

void initProfiler() {
    matdash::add_hook<&CCApplication_setupVerticalSync>(CC_ADDR("?setupVerticalSync@CCApplication@cocos2d@@QAEXXZ"));
}

#endif
