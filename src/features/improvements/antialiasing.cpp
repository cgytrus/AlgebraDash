#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCEGLView.hpp>

#ifdef GEODE_IS_WINDOWS
struct Antialiasing : geode::Modify<Antialiasing, CCEGLView> {
    void setupWindow(CCRect rect) {
        auto setting = Mod::get()->getSettingValue<int64_t>("antialiasing");
        auto _glfwWindowHint = reinterpret_cast<void(*)(int, int)>(base::getCocos() + 0x1116f0);
        _glfwWindowHint(GLFW_SAMPLES, static_cast<int>(setting));
        CCEGLView::setupWindow(rect);
    }
};
#endif
