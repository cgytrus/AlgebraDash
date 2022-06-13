#include "../../includes.h"
#include "../../ThreadPool.hpp"

#include <queue>

struct queuedCall {
    gd::GameObject* self;
    ccColor3B* color;
    void (__thiscall* function)(gd::GameObject*, ccColor3B*);
};
std::queue<queuedCall> queuedCalls;

void (__thiscall* GameObject_setObjectColor)(gd::GameObject*, ccColor3B*);
void (__thiscall* AnimatedGameObject_setObjectColor)(gd::GameObject*, ccColor3B*);
void (__thiscall* LabelGameObject_setObjectColor)(gd::GameObject*, ccColor3B*);

bool inTargetFunc = false;
void (__thiscall* PlayLayer_updateVisibility)(gd::PlayLayer*);
void __fastcall PlayLayer_updateVisibility_H(gd::PlayLayer* self) {
    ZoneScoped
    inTargetFunc = true;
    PlayLayer_updateVisibility(self);
    inTargetFunc = false;
    while(!queuedCalls.empty()) {
        auto call = queuedCalls.front();
        call.function(call.self, call.color);
        queuedCalls.pop();
    }
}

void __fastcall GameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(inTargetFunc)
    //    queuedCalls.push({self, color, GameObject_setObjectColor});
    //else
        GameObject_setObjectColor(self, color);
}

void __fastcall AnimatedGameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(!inTargetFunc)
    //    queuedCalls.push({self, color, AnimatedGameObject_setObjectColor});
    //else
        AnimatedGameObject_setObjectColor(self, color);
}

void __fastcall LabelGameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(!inTargetFunc)
    //    queuedCalls.push({self, color, LabelGameObject_setObjectColor});
    //else
        LabelGameObject_setObjectColor(self, color);
}

#include "playLayerUpdateVisibility.h"
void initPlayLayerUpdateVisibilityOptimizations(uintptr_t base) {
    MH_CreateHook(reinterpret_cast<void*>(base + 0x205460),
        reinterpret_cast<void*>(&PlayLayer_updateVisibility_H),
        reinterpret_cast<void**>(&PlayLayer_updateVisibility));

    MH_CreateHook(reinterpret_cast<void*>(base + 0xee620),
        reinterpret_cast<void*>(&GameObject_setObjectColor_H),
        reinterpret_cast<void**>(&GameObject_setObjectColor));

    MH_CreateHook(reinterpret_cast<void*>(base + 0x253570),
        reinterpret_cast<void*>(&AnimatedGameObject_setObjectColor_H),
        reinterpret_cast<void**>(&AnimatedGameObject_setObjectColor));

    MH_CreateHook(reinterpret_cast<void*>(base + 0x25ba30),
        reinterpret_cast<void*>(&LabelGameObject_setObjectColor_H),
        reinterpret_cast<void**>(&LabelGameObject_setObjectColor));
}