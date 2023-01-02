#pragma once

#pragma warning(push, 0)
#include <cocos2d.h>
#pragma warning(pop)
#include <filesystem>
#include <fstream>
#include <extensions2.h>

#include <util/reverse.hpp>
#include <optimizations/base.hpp>

namespace ad {
    void addHooks();
}

namespace ad::config {
    static bool enabled = true;

    static std::filesystem::path getConfigPath() {
        auto configDir = std::filesystem::path(cocos2d::CCFileUtils::sharedFileUtils()->getWritablePath2()) / "config";
        std::filesystem::create_directories(configDir);
        return configDir / "AlgebraDash.txt";
    }
    static void loadConfig() {
        auto configPath = getConfigPath();
        if(!std::filesystem::exists(configPath))
            return;
        std::ifstream config(configPath);
        config >> enabled;
        for(auto opt : optimizations) {
            opt->load(config);
        }
        config.close();
    }
    static void saveConfig() {
        std::ofstream config(getConfigPath());
        config << enabled;
        for(auto opt : optimizations) {
            config << ' ';
            opt->save(config);
        }
        config.close();
    }

    static bool _wasEverEnabled = false;
    static void enableHooks() {
        if(_wasEverEnabled) {
            MH_EnableHook(MH_ALL_HOOKS);
            return;
        }
        _wasEverEnabled = true;
        addHooks();
    }
    static void disableHooks() {
        if(_wasEverEnabled)
            MH_DisableHook(MH_ALL_HOOKS);
    }

    template<bool& prop>
    static void addCheckbox(MegaHackExt::Window& window, const char* label) {
        auto checkbox = MegaHackExt::CheckBox::Create(label);
        checkbox->set(prop);
        checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool value) {
            prop = value;
            saveConfig();
        });
        window.add(checkbox);
    }

    static void initMegahack() {
        if(!GetModuleHandle(TEXT("hackpro.dll")))
            return;

        auto window = MegaHackExt::Window::Create("Algebra Dash");

        for(auto opt : reverse(optimizations)) {
            opt->initMegahack(*window);
        }

        auto checkbox = MegaHackExt::CheckBox::Create("Enabled");
        checkbox->set(enabled);
        checkbox->setCallback([](MegaHackExt::CheckBox* obj, bool value) {
            enabled = value;
            if(value)
                enableHooks();
            else
                disableHooks();
            saveConfig();
        });
        window->add(checkbox);

        MegaHackExt::Client::commit(window);
    }
}
