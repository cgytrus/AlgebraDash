#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCEGLView.hpp>

#ifdef GEODE_IS_WINDOWS
struct Antialiasing : geode::Modify<Antialiasing, CCEGLView> {
    void setupWindow(CCRect rect) {
        auto setting = Mod::get()->getSettingValue<int64_t>("antialiasing");
        int samples = 0;
        switch(setting) {
            case 1:
                samples = 2;
                break;
            case 2:
                samples = 4;
                break;
            case 3:
                samples = 8;
                break;
            case 4:
                samples = 16;
                break;
        }
        auto _glfwWindowHint = reinterpret_cast<void(*)(int, int)>(base::getCocos() + 0x1116f0);
        _glfwWindowHint(GLFW_SAMPLES, samples);
        CCEGLView::setupWindow(rect);
    }
};
#endif
