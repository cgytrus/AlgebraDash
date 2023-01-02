#ifdef TRACY_ENABLE

#include "../../includes.h"

void initTracyHooks();
matdash::cc::thiscall<int> CCApplication_setupVerticalSync(CCApplication* self) {
    initTracyHooks();
    return matdash::orig<&CCApplication_setupVerticalSync>(self);
}

matdash::cc::thiscall<void> CCEGLView_swapBuffers(CCEGLView* self) {
    ZoneScopedN("CCEGLView::swapBuffers");
    matdash::orig<&CCEGLView_swapBuffers>(self);
    FrameMark;
    return {};
}

template<template<class> class Conv, class Ret, const char* Name, class... Args>
Conv<Ret> profilerHook(Args&... args) {
    ZoneScopedN(Name);
    if constexpr (std::is_same_v<Ret, void>) {
        matdash::orig<&profilerHook<Conv, Ret, Name, Args...>>(std::forward<Args>(args)...);
        return {};
    }
    else
        return matdash::orig<&profilerHook<Conv, Ret, Name, Args...>>(std::forward<Args>(args)...);
}

#define GD gd::base
#define COCOS2D reinterpret_cast<uintptr_t>(ccModule)
#define COCOS2DSYM(name) CC_ADDR(name)
#define PROFILER_HOOK(address, ret, conv, name, ...) { \
    static char nameStr[] = #name; \
    matdash::add_hook<&profilerHook<matdash::cc::conv, ret, nameStr, __VA_ARGS__>>(address); \
}
void initTracyHooks() {
    // frame
    matdash::add_hook<&CCEGLView_swapBuffers>(CC_ADDR("?swapBuffers@CCEGLView@cocos2d@@UAEXXZ"));

    #include "hooks.h"
}

void initProfiler() {
    matdash::add_hook<&CCApplication_setupVerticalSync>(CC_ADDR("?setupVerticalSync@CCApplication@cocos2d@@QAEXXZ"));
}

#endif
