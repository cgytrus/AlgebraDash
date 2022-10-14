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

void (__thiscall* updateAtlasIndex)(CCSpriteBatchNode*, CCSprite*, int*);
void (__thiscall* CCSpriteBatchNode_sortAllChildren)(CCSpriteBatchNode*);
void __fastcall CCSpriteBatchNode_sortAllChildren_H(CCSpriteBatchNode* self) {
    ZoneScoped;

    auto m_bReorderChildDirty = (bool*)((uintptr_t)self + 0xda);
    auto m_bManualSortChildren = (bool*)((uintptr_t)self + 0x100);
    auto m_bManualSortAllChildrenDirty = (bool*)((uintptr_t)self + 0x101);

    if(!*m_bReorderChildDirty || *m_bManualSortChildren && !*m_bManualSortAllChildrenDirty)
        return;

    CCArray* children = self->getChildren();
    CCNode** arr = (CCNode**)children->data->arr;

    if(children->count() == 0)
        return;

    // radix sort (vanilla uses insertion sort)
    {
        ZoneScopedN("sort");

        size_t length = (size_t)children->data->num;

        int minZOrder = INT_MAX;
        int maxZOrder = INT_MIN;
        unsigned int maxOrderOfArrival = 0;
        for(size_t i = 0; i < length; i++) {
            int zOrder = arr[i]->getZOrder();
            if(zOrder > maxZOrder)
                maxZOrder = zOrder;
            if(zOrder < minZOrder)
                minZOrder = zOrder;

            unsigned int orderOfArrival = arr[i]->getOrderOfArrival();
            if(orderOfArrival > maxOrderOfArrival)
                maxOrderOfArrival = orderOfArrival;
        }
        maxZOrder -= minZOrder;
        maxZOrder *= maxOrderOfArrival;

        CCNode** out = new CCNode*[length];
        for(int exp = 1; maxZOrder / exp > 0; exp *= 10) {
            int count[10] = { 0 };
            for(size_t i = 0; i < length; i++) {
                CCNode* elem = arr[i];
                auto countIndex = (((elem->getZOrder() - minZOrder) * maxOrderOfArrival + elem->getOrderOfArrival()) / exp) % 10;
                count[countIndex]++;
            }
            for(size_t i = 1; i < 10; i++)
                count[i] += count[i - 1];
            for(size_t i = 1; i <= length; i++) {
                CCNode* elem = arr[length - i];
                auto countIndex = (((elem->getZOrder() - minZOrder) * maxOrderOfArrival + elem->getOrderOfArrival()) / exp) % 10;
                out[count[countIndex] - 1] = elem;
                count[countIndex]--;
            }
            for(size_t i = 0; i < length; i++)
                arr[i] = out[i];
        }
        delete out;
    }

    // sorted, now check all children
    {
        ZoneScopedN("update indicies");
        // first sort all children recursively based on zOrder
        arrayMakeObjectsPerformSelector(children, sortAllChildren, CCSprite*);

        int index = 0;

        CCObject* pObj = NULL;
        // fast dispatch, give every child a new atlasIndex based on their relative zOrder (keep parent -> child relations intact)
        // and at the same time reorder descendants and the quads to the right index
        CCARRAY_FOREACH(children, pObj) {
            CCSprite* pChild = (CCSprite*)pObj;
            updateAtlasIndex(self, pChild, &index);
        }
    }

    *m_bReorderChildDirty = false;
    *m_bManualSortAllChildrenDirty = false;
}

#include "rendering.h"
void initRenderingOptimizations(HMODULE cocos2dModule) {
    MH_CreateHook(reinterpret_cast<void*>(GetProcAddress(cocos2dModule, "?draw@CCSpriteBatchNode@cocos2d@@UAEXXZ")),
        reinterpret_cast<void*>(&CCSpriteBatchNode_draw_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_draw));

    MH_CreateHook(reinterpret_cast<void*>(GetProcAddress(cocos2dModule, "?sortAllChildren@CCSpriteBatchNode@cocos2d@@UAEXXZ")),
        reinterpret_cast<void*>(&CCSpriteBatchNode_sortAllChildren_H),
        reinterpret_cast<void**>(&CCSpriteBatchNode_sortAllChildren));

    updateAtlasIndex = reinterpret_cast<void (__thiscall *)(cocos2d::CCSpriteBatchNode*, cocos2d::CCSprite*, int*)>
        (GetProcAddress(cocos2dModule, "?updateAtlasIndex@CCSpriteBatchNode@cocos2d@@AAEXPAVCCSprite@2@PAH@Z"));
}
