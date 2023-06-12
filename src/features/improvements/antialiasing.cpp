#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCEGLView.hpp>

// TODO: mac address for setupWindow
#ifdef GEODE_IS_WINDOWS
struct Antialiasing : geode::Modify<Antialiasing, CCEGLView> {
    void setupWindow(CCRect rect) {
        auto _glfwWindowHint = reinterpret_cast<void(*)(int, int)>(base::getCocos() + 0x1116f0);
        // hardcoded to msaa x8 for now, will add config later
        _glfwWindowHint(GLFW_SAMPLES, 8);
        CCEGLView::setupWindow(rect);
    }
};
#endif
