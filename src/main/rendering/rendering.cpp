#include <Tracy.hpp>
#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#include "../../ThreadPool.hpp"

// doesn't work because not in cocos2d bindings
class $modify(CCSpriteBatchNode) {
    void draw() {
        ZoneScoped

        if(getTextureAtlas()->getTotalQuads() == 0)
            return;

        auto shader = getShaderProgram();
        ccGLEnable(getGLServerState());
        shader->use();
        shader->setUniformsForBuiltins();

        if(getChildrenCount() > 0) {
            auto threadPool = ThreadPool::sharedPool();
            CCArray* children = getChildren();
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

        auto blendFunc = getBlendFunc();
        ccGLBlendFunc(blendFunc.src, blendFunc.dst);

        getTextureAtlas()->drawQuads();
    }
};
