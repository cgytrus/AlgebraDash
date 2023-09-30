#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/AppDelegate.hpp>

bool isUnfocusing = false;
struct FastAltTab : geode::Modify<FastAltTab, AppDelegate> {
    void applicationDidEnterBackground() {
        isUnfocusing = true;
        AppDelegate::applicationDidEnterBackground();
        isUnfocusing = false;
    }
    void trySaveGame() {
        if(Mod::get()->getSettingValue<bool>("fast-alt-tab") && isUnfocusing) {
            log::debug("fast alt tab: skipping save");
            return;
        }
        AppDelegate::trySaveGame();
    }

    static void onModify(auto& self) {
        if(!self.setHookPriority("AppDelegate::trySaveGame", -9999))
            log::warn("fast alt tab: failed to set hook priority, some stuff might still save");
    }
};
