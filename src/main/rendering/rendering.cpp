#include "../../includes.h"
#include "../../ThreadPool.hpp"

void (__thiscall* CCSpriteBatchNode_draw)(CCSpriteBatchNode*);
void __fastcall CCSpriteBatchNode_draw_H(CCSpriteBatchNode* self) {
    ZoneScoped

    if(self->getTextureAtlas()->getTotalQuads() == 0)
        return;

    auto shader = self->getShaderProgram();
    ccGLEnable(self->getGLServerState());
    shader->use();
    shader->setUniformsForBuiltins();

    if(self->getChildrenCount() > 0) {
        auto threadPool = ThreadPool::sharedPool();
        CCArray* children = self->getChildren();
        CCObject** globalStart = children->data->arr;
        CCObject** globalEnd = globalStart + children->data->num;
        for(; globalStart < globalEnd; globalStart += RENDER_THREAD_BATCH_COUNT) {
            threadPool->queueJob([globalStart, globalEnd] {
                ZoneScopedN("update transforms job")
                CCObject** currentStart = globalStart;
                CCObject** currentEnd = globalStart + RENDER_THREAD_BATCH_COUNT;
                for(; currentStart < currentEnd && currentStart < globalEnd; ++currentStart) {
                    CCSprite* sprite = (CCSprite*)*currentStart;
                    if(!sprite)
                        continue;
                    sprite->updateTransform();
                }
            });
        }
        threadPool->finishQueue();
        threadPool->waitForAllJobs();
    }

    auto blendFunc = self->getBlendFunc();
    ccGLBlendFunc(blendFunc.src, blendFunc.dst);

    self->getTextureAtlas()->drawQuads();
}

#include "rendering.h"
void initRenderingOptimizations(uintptr_t cocos2dBase) {
    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd5bd0),
        reinterpret_cast<void*>(&CCSpriteBatchNode_draw_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_draw));
}
