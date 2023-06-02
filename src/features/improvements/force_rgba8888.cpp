#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCTexture2D.hpp>

struct ForceRGBA8888 : geode::Modify<ForceRGBA8888, CCTexture2D> {
    static void setDefaultAlphaPixelFormat(CCTexture2DPixelFormat format) {
        CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA8888);
    }
    static CCTexture2DPixelFormat defaultAlphaPixelFormat() {
        return kCCTexture2DPixelFormat_RGBA8888;
    }
};