#include "../../includes.h"
#include "../../ThreadPool.hpp"

constexpr size_t threadBatchCount = RENDER_THREAD_BATCH_COUNT;
std::mutex jobCounterMutex;
unsigned int jobCounter = 0;
void (__thiscall* CCSpriteBatchNode_draw)(CCSpriteBatchNode*);
void __fastcall CCSpriteBatchNode_draw_H(CCSpriteBatchNode* self) {
    ZoneScoped

    auto m_pobTextureAtlas = (CCTextureAtlas**)((uintptr_t)self + 0xf0);
    if((*m_pobTextureAtlas)->getTotalQuads() == 0)
        return;

    auto shader = self->getShaderProgram();
    ccGLEnable(self->getGLServerState());
    shader->use();
    shader->setUniformsForBuiltins();

    auto threadPool = ThreadPool::sharedPool();
    if(self->getChildrenCount() > 0) {
        CCArray* children = self->getChildren();
        CCObject** globalStart = children->data->arr;
        CCObject** globalEnd = globalStart + children->data->num;
        for(; globalStart < globalEnd; globalStart += 10) {
            {
                std::unique_lock<std::mutex> lock(jobCounterMutex);
                ++jobCounter;
            }
            threadPool->queueJob([globalStart, globalEnd] {
                ZoneScopedN("update transforms job")
                CCObject** currentStart = globalStart;
                CCObject** currentEnd = globalStart + threadBatchCount;
                for(; currentStart < currentEnd && currentStart < globalEnd; ++currentStart) {
                    CCSprite* sprite = (CCSprite*)*currentStart;
                    if(!sprite)
                        continue;
                    sprite->updateTransform();
                }
                {
                    std::unique_lock<std::mutex> lock(jobCounterMutex);
                    --jobCounter;
                }
            });
        }
    }

    while(jobCounter > 0)
        threadPool->tryExecuteJob();

    auto m_blendFunc = (ccBlendFunc*)((uintptr_t)self + 0xf4);
    ccGLBlendFunc((*m_blendFunc).src, (*m_blendFunc).dst);

    (*m_pobTextureAtlas)->drawQuads();
}

#include "rendering.h"
void initRenderingOptimizations(uintptr_t cocos2dBase) {
    MH_CreateHook(reinterpret_cast<void*>(cocos2dBase + 0xd5bd0),
        reinterpret_cast<void*>(&CCSpriteBatchNode_draw_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_draw));
}
