#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCSpriteBatchNode.hpp>

// TODO: mac address for CCSpriteBatchNode::updateAtlasIndex
#ifdef GEODE_IS_WINDOWS
class meWhen;
template<>
struct geode::modifier::ModifyDerive<meWhen, meWhen> {
    static void updateAtlasIndex(CCSpriteBatchNode* self, CCSprite* sprite, int* curIndex) {
        self->updateAtlasIndex(sprite, curIndex);
    }
};
struct FastSpriteBatchNodeSort : geode::Modify<FastSpriteBatchNodeSort, CCSpriteBatchNode> {
    void CCSpriteBatchNode_sortAllChildren(CCSpriteBatchNode* self) {
        ZoneScoped;

        if(!m_bReorderChildDirty || m_bManualSortChildren && !m_bManualSortAllChildrenDirty)
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
                geode::modifier::ModifyDerive<meWhen, meWhen>::updateAtlasIndex(this, pChild, &index);
            }
        }

        m_bReorderChildDirty = false;
        m_bManualSortAllChildrenDirty = false;
    }

    static void onModify(auto& self) {
        // TODO
        //self.setAutoEnable(false);
        //self.setEnabled(true);
    }
};
#endif
