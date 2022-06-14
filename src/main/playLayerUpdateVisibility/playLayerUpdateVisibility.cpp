#include "../../includes.h"
#include "../../patcher.h"
#include "../../ThreadPool.hpp"

#include <mutex>

/*#include <queue>

struct queuedCall {
    gd::GameObject* self;
    ccColor3B* color;
    void (__thiscall* function)(gd::GameObject*, ccColor3B*);
};
std::queue<queuedCall> queuedCalls;

void (__thiscall* GameObject_setObjectColor)(gd::GameObject*, ccColor3B*);
void (__thiscall* AnimatedGameObject_setObjectColor)(gd::GameObject*, ccColor3B*);
void (__thiscall* LabelGameObject_setObjectColor)(gd::GameObject*, ccColor3B*);*/

gd::PlayLayer* playLayer;
int startSection;
int endSection;
void (__thiscall* PlayLayer_updateVisibility)(gd::PlayLayer*);
void __fastcall PlayLayer_updateVisibility_H(gd::PlayLayer* self) {
    ZoneScoped
    playLayer = self;
    startSection = (int)(floor(self->m_cameraX / 100.f) - 1.f);
    endSection = (int)(ceil((self->m_cameraX + CCDirector::sharedDirector()->getWinSize().width) / 100.f) + 1.f);
    PlayLayer_updateVisibility(self);
    playLayer = nullptr;
    /*while(!queuedCalls.empty()) {
        auto call = queuedCalls.front();
        call.function(call.self, call.color);
        queuedCalls.pop();
    }*/
}

std::unordered_map<void*, std::mutex*> instanceMutexes;

gd::GameObject* (__thiscall* GameObject_ctor)(gd::GameObject*);
gd::GameObject* __fastcall GameObject_ctor_H(gd::GameObject* self) {
    auto mutex = new std::mutex();
    instanceMutexes.emplace(self, mutex);
    return GameObject_ctor(self);
}
void (__thiscall* GameObject_dector)(gd::GameObject*);
void __fastcall GameObject_dector_H(gd::GameObject* self) {
    GameObject_dector(self);
    delete instanceMutexes[self];
    instanceMutexes.erase(self);
}

CCNode* (__thiscall* CCNode_ctor)(CCNode*);
CCNode* __fastcall CCNode_ctor_H(CCNode* self) {
    auto mutex = new std::mutex();
    instanceMutexes.emplace(self, mutex);
    return CCNode_ctor(self);
}
void (__thiscall* CCNode_dector)(CCNode*);
void __fastcall CCNode_dector_H(CCNode* self) {
    CCNode_dector(self);
    delete instanceMutexes[self];
    instanceMutexes.erase(self);
}

CCNode* (__thiscall* CCSpriteBatchNode_ctor)(CCNode*);
CCNode* __fastcall CCSpriteBatchNode_ctor_H(CCNode* self) {
    auto mutex = new std::mutex();
    instanceMutexes.emplace(self, mutex);
    return CCNode_ctor(self);
}
void (__thiscall* CCSpriteBatchNode_dector)(CCSpriteBatchNode*);
void __fastcall CCSpriteBatchNode_dector_H(CCSpriteBatchNode* self) {
    CCSpriteBatchNode_dector(self);
    delete instanceMutexes[self];
    instanceMutexes.erase(self);
}

void (__thiscall* CCSpriteBatchNode_removeSpriteFromAtlas)(CCNode*, CCSprite*);
void __fastcall CCSpriteBatchNode_removeSpriteFromAtlas_H(CCNode* self, void*, CCSprite* sprite) {
    ZoneScoped
    {
        std::unique_lock<std::mutex> lock(*instanceMutexes[self]);
        CCSpriteBatchNode_removeSpriteFromAtlas(self, sprite);
    }
}
void (__thiscall* CCNode_removeChild)(CCNode*, CCNode*, bool idk);
void __fastcall CCNode_removeChild_H(CCNode* self, void*, CCNode* child, bool idk) {
    ZoneScoped
    {
        std::unique_lock<std::mutex> lock(*instanceMutexes[self]);
        CCNode_removeChild(self, child, idk);
    }
}
void deactivateObject(gd::GameObject* self, bool idk) {
    ZoneScoped
    {
        std::unique_lock<std::mutex> lock(*instanceMutexes[self]);
        (*(void(__thiscall**)(gd::GameObject*, bool))((uintptr_t)*(void**)self + 0x22c))(self, idk);
    }
}

void (__thiscall* CCArray_removeAllObjects)(CCArray*);
void __fastcall CCArray_removeAllObjects_H(CCArray* self) {
    ZoneScoped
    if(!playLayer)
        return CCArray_removeAllObjects(self);

    if(self == playLayer->unk29C) { // pre-last loop
        auto threadPool = ThreadPool::sharedPool();

        if(!playLayer->unk310) {
            int arraysCount = playLayer->m_pObjectContainerArrays->count();
            for(int i = playLayer->m_nCurrectSection; i <= playLayer->unk2BC; ++i) {
                if(i < 0 || i >= arraysCount)
                    continue;
                threadPool->queueJob([i] {
                    ZoneScopedN("deactivate objects false job")
                    CCArray* objectContainers = (CCArray*)playLayer->m_pObjectContainerArrays->objectAtIndex(i);
                    if(!objectContainers)
                        return;
                    CCObject* obj;
                    CCARRAY_FOREACH(objectContainers, obj) {
                        gd::GameObject* gameObject = (gd::GameObject*)obj;
                        if(!gameObject)
                            continue;
                        deactivateObject(gameObject, false);
                    }
                });
            }
        }
        /*CCObject* obj;
        CCARRAY_FOREACH(self, obj) {
            gd::GameObject* gameObject = (gd::GameObject*)obj;
            if(!gameObject)
                continue;
            if(gameObject->m_nSection < startSection || gameObject->m_nSection > endSection)
                deactivateObject(gameObject, true);
        }*/
        if(self && self->data->num > 0) {
            CCObject** globalStart = self->data->arr;
            CCObject** globalEnd = globalStart + self->data->num;
            for(; globalStart < globalEnd; globalStart += 10) {
                threadPool->queueJob([globalStart, globalEnd] {
                    ZoneScopedN("deactivate objects true job")
                    CCObject** currentStart = globalStart;
                    CCObject** currentEnd = globalStart + 10;
                    for(; currentStart < currentEnd && currentStart < globalEnd; ++currentStart) {
                        gd::GameObject* gameObject = (gd::GameObject*)*currentStart;
                        if(!gameObject)
                            continue;
                        if(gameObject->m_nSection < startSection || gameObject->m_nSection > endSection)
                            deactivateObject(gameObject, true);
                    }
                });
            }
        }

        threadPool->finishQueue();
        threadPool->waitForAllJobs();
    }
    else if(self == playLayer->unk35C) { // last loop (updates colors)

    }

    CCArray_removeAllObjects(self);
}

/*void __fastcall GameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(inUpdateVisibility)
    //    queuedCalls.push({self, color, GameObject_setObjectColor});
    //else
        GameObject_setObjectColor(self, color);
}

void __fastcall AnimatedGameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(!inUpdateVisibility)
    //    queuedCalls.push({self, color, AnimatedGameObject_setObjectColor});
    //else
        AnimatedGameObject_setObjectColor(self, color);
}

void __fastcall LabelGameObject_setObjectColor_H(gd::GameObject* self, void*, ccColor3B* color) {
    ZoneScoped
    //if(!inUpdateVisibility)
    //    queuedCalls.push({self, color, LabelGameObject_setObjectColor});
    //else
        LabelGameObject_setObjectColor(self, color);
}*/

#include "playLayerUpdateVisibility.h"
void initPlayLayerUpdateVisibilityOptimizations(uintptr_t base, uintptr_t cocos2dBase) {
    MH_CreateHook(reinterpret_cast<void*>(base + 0x205460),
        reinterpret_cast<void*>(&PlayLayer_updateVisibility_H),
        reinterpret_cast<void**>(&PlayLayer_updateVisibility));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0x62ce0),
        reinterpret_cast<void*>(&CCArray_removeAllObjects_H),
        reinterpret_cast<void**>(&CCArray_removeAllObjects));

    // skip the pre-pre-last and pre-last loops
    patch(base + 0x205c0e, { 0xeb });
    patch(base + 0x205c83, { 0xeb });

    MH_CreateHook(reinterpret_cast<void*>(base + 0x983e0),
        reinterpret_cast<void*>(&GameObject_ctor_H),
        reinterpret_cast<void**>(&GameObject_ctor));
    MH_CreateHook(reinterpret_cast<void*>(base + 0xcf340),
        reinterpret_cast<void*>(&GameObject_dector_H),
        reinterpret_cast<void**>(&GameObject_dector));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0x5e7d0),
        reinterpret_cast<void*>(&CCNode_ctor_H),
        reinterpret_cast<void**>(&CCNode_ctor));
    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0x5ea40),
        reinterpret_cast<void*>(&CCNode_dector_H),
        reinterpret_cast<void**>(&CCNode_dector));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd5530),
        reinterpret_cast<void*>(&CCSpriteBatchNode_ctor_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_ctor));
    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd5580),
        reinterpret_cast<void*>(&CCSpriteBatchNode_dector_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_dector));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0x5fe50),
        reinterpret_cast<void*>(&CCNode_removeChild_H),
        reinterpret_cast<void**>(&CCNode_removeChild));

    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd6560),
        reinterpret_cast<void*>(&CCSpriteBatchNode_removeSpriteFromAtlas_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_removeSpriteFromAtlas));

    /*MH_CreateHook(reinterpret_cast<void*>(base + 0xee620),
        reinterpret_cast<void*>(&GameObject_setObjectColor_H),
        reinterpret_cast<void**>(&GameObject_setObjectColor));

    MH_CreateHook(reinterpret_cast<void*>(base + 0x253570),
        reinterpret_cast<void*>(&AnimatedGameObject_setObjectColor_H),
        reinterpret_cast<void**>(&AnimatedGameObject_setObjectColor));

    MH_CreateHook(reinterpret_cast<void*>(base + 0x25ba30),
        reinterpret_cast<void*>(&LabelGameObject_setObjectColor_H),
        reinterpret_cast<void**>(&LabelGameObject_setObjectColor));*/
}