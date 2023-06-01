#ifdef TRACY_ENABLE

#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/Modify.hpp>

class $modify(CCEGLView) {
    void swapBuffers() {
        ZoneScopedN("CCEGLView::swapBuffers");
        CCEGLView::swapBuffers();
        FrameMark;
    }
};

#define PROFILER_HOOK_3(Ret_, Class_, Name_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_() { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(); } };
#define PROFILER_HOOK_4(Ret_, Class_, Name_, A_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a); } };
#define PROFILER_HOOK_5(Ret_, Class_, Name_, A_, B_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b); } };
#define PROFILER_HOOK_6(Ret_, Class_, Name_, A_, B_, C_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c); } };
#define PROFILER_HOOK_7(Ret_, Class_, Name_, A_, B_, C_, D_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d); } };
#define PROFILER_HOOK_8(Ret_, Class_, Name_, A_, B_, C_, D_, E_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d, E_ e) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d, e); } };
#define PROFILER_HOOK_9(Ret_, Class_, Name_, A_, B_, C_, D_, E_, F_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d, E_ e, F_ f) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d, e, f); } };
#define PROFILER_HOOK(...) GEODE_INVOKE(GEODE_CONCAT(PROFILER_HOOK_, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

#include <profiler/hooks.hpp>

#endif
