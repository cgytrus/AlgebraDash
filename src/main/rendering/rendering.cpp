#include "../../includes.h"

void (__thiscall* CCSpriteBatchNode_draw)(CCSpriteBatchNode*);
void __fastcall CCSpriteBatchNode_draw_H(CCSpriteBatchNode* self) {
    ZoneScoped;

    if(self->getTextureAtlas()->getTotalQuads() == 0)
        return;

    auto shader = self->getShaderProgram();
    ccGLEnable(self->getGLServerState());
    shader->use();
    shader->setUniformsForBuiltins();

    if(self->getChildrenCount() > 0) {
        auto threadPool = sharedPool();
        CCArray* children = self->getChildren();
        CCObject** objects = children->data->arr;
        BS::multi_future<void> updateTransformsJob =
            threadPool->parallelize_loop(0, children->data->num, [objects](unsigned int a, unsigned int b) {
                ZoneScopedN("update transforms job");
                for(unsigned int i = a; i < b; ++i) {
                    CCSprite* sprite = (CCSprite*)objects[i];
                    if(!sprite)
                        continue;
                    sprite->updateTransform();
                }
            });
        {
            ZoneScopedN("wait");
            updateTransformsJob.wait();
        }
    }

    auto blendFunc = self->getBlendFunc();
    ccGLBlendFunc(blendFunc.src, blendFunc.dst);

    self->getTextureAtlas()->drawQuads();
}

#include "rendering.h"
void initRenderingOptimizations(HMODULE cocos2dModule) {
    MH_CreateHook(reinterpret_cast<void*>(GetProcAddress(cocos2dModule, "?draw@CCSpriteBatchNode@cocos2d@@UAEXXZ")),
        reinterpret_cast<void*>(&CCSpriteBatchNode_draw_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_draw));
}
