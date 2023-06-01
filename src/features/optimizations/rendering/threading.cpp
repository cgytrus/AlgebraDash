#include <includes.hpp>

#include <Geode/modify/CCSpriteBatchNode.hpp>

class meWhen;
template<>
struct geode::modifier::ModifyDerive<meWhen, meWhen> {
    static void updateAtlasIndex(CCSpriteBatchNode* self, CCSprite* sprite, int* curIndex) {
        self->updateAtlasIndex(sprite, curIndex);
    }
};
class $modify(CCSpriteBatchNode) {
    void draw() {
        ZoneScoped;

        if(getTextureAtlas()->getTotalQuads() == 0)
            return;

        auto shader = getShaderProgram();
        ccGLEnable(getGLServerState());
        shader->use();
        shader->setUniformsForBuiltins();

        if(getChildrenCount() > 0) {
            auto threadPool = sharedPool();
            CCArray* children = getChildren();
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

        auto blendFunc = getBlendFunc();
        ccGLBlendFunc(blendFunc.src, blendFunc.dst);

        getTextureAtlas()->drawQuads();
    }

    static void onModify(auto& self) {
        // TODO
        //self.setAutoEnable(false);
        //self.setEnabled(true);
    }
};
