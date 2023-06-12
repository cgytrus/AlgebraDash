#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCSpriteBatchNode.hpp>

#include <shared_pool.hpp>

// TODO: mac addresses for ccGLEnable and CCTextureAtlas::drawQuads
#ifdef GEODE_IS_WINDOWS
struct ThreadedSpriteBatchNode : geode::Modify<ThreadedSpriteBatchNode, CCSpriteBatchNode> {
    void draw() {
        ZoneScoped;

        if(getTextureAtlas()->getTotalQuads() == 0)
            return;

        auto shader = getShaderProgram();
        ccGLEnable(getGLServerState());
        shader->use();
        shader->setUniformsForBuiltins();

        if(getChildrenCount() > 0) {
            CCArray* children = getChildren();
            CCObject** objects = children->data->arr;
            BS::multi_future<void> updateTransformsJob =
                sharedPool().parallelize_loop(0, children->data->num, [objects](unsigned int a, unsigned int b) {
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
#endif
