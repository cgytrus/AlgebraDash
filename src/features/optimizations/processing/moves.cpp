#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <numbers>
#include <shared_pool.hpp>

#ifdef GEODE_IS_WINDOWS
static TracyLockable(std::mutex, objectsVecMutex);
static TracyLockable(std::mutex, disabledObjectsMutex);
static TracyLockable(std::mutex, reorderObjectSectionMutex);
static TracyLockable(std::mutex, processedGroupsMutex);
struct FastMoves : geode::Modify<FastMoves, GJBaseGameLayer> {
    void processMoveActions() {
        ZoneScoped;

        CCDictElement* el;
        auto dict = m_effectManager->m_moveNodes;
        CCDICT_FOREACH(dict, el) {
            ZoneScopedN("m_moveNodes iteration");
            auto moveNode = (CCMoveCNode*)el->getObject();
            int moveTag = moveNode->getTag();

            auto staticObjects = m_staticGroups[moveTag];
            if(!staticObjects) {
                staticObjects = CCArray::create();
                m_staticGroupDict->setObject(staticObjects, moveTag);
                m_staticGroups[moveTag] = staticObjects;
            }

            auto optimizedObjects = m_optimizedGroups[moveTag];
            if(!optimizedObjects) {
                optimizedObjects = CCArray::create();
                m_optimizedGroupDict->setObject(optimizedObjects, moveTag);
                m_optimizedGroups[moveTag] = optimizedObjects;
            }

            processMovesInner(staticObjects, moveNode->m_staticMoveX, moveNode->m_staticMoveY, moveNode);
            processMovesInner(optimizedObjects, moveNode->m_optimizedMoveX, moveNode->m_optimizedMoveY, moveNode);
        }
    }

    void processMovesInner(CCArray* objects, float moveX, float moveY, CCMoveCNode* moveNode) {
        ZoneScoped;
        if (!moveNode->m_forceUpdate && moveX == 0.f && moveY == 0.f)
            return;
        CCObjectType moveObjType = moveNode->getObjType();
        CCObject** arr = objects->data->arr;
        BS::multi_future<void> moveJob = sharedPool().parallelize_loop(0, objects->data->num,
            [this, arr, moveX, moveY, moveObjType](unsigned int a, unsigned int b) {
                ZoneScopedN("move job");
                for(unsigned int i = a; i < b; ++i) {
                    auto* obj = (GameObject*)arr[i];
                    if(!obj)
                        break;
                    moveObject(obj, moveX, moveY, moveObjType);
                }
            });
        {
            ZoneScopedN("wait");
            moveJob.wait();
        }
    }

    void moveObject(GameObject* obj, float moveX, float moveY, CCObjectType moveObjType) {
        ZoneScoped;

        if (!obj->m_inOptimizedGroup) {
            if (!obj->m_queuedForPositionUpdate) {
                obj->m_firstPosition = obj->m_startPosition + obj->m_startPosOffset;
                obj->m_queuedForPositionUpdate = true;
                {
                    std::scoped_lock lock(objectsVecMutex);
                    m_objectsVec.push_back(obj);
                }
                obj->m_isObjectRectDirty = true;
                obj->m_isOrientedRectDirty = true;
            }
            if (moveObjType == CCObjectType::GameObject)
                obj->m_shouldUpdateFirstPosition = true;
        }
        obj->m_textureRectDirty = true;

        if (moveY != 0.f)
            obj->m_startPosOffset.y += moveY;
        if (moveX != 0.f && !obj->m_tintTrigger) {
            obj->m_startPosOffset.x += moveX;
            checkSectionChange(obj);
        }

        if (obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
            obj->m_queuedForPositionUpdate = true;
            {
                std::scoped_lock lock(disabledObjectsMutex);
                m_disabledObjects.push_back(obj);
            }
        }
    }

    void checkSectionChange(GameObject* obj) {
        ZoneScoped;

        int oldSection = obj->m_section;
        int newSection = std::max((int)((obj->m_startPosition.x + obj->m_startPosOffset.x) / 100.f), 0);
        if(newSection == oldSection)
            return;
        {
            std::scoped_lock lock(reorderObjectSectionMutex);
            reorderObjectSection(obj);
        }
        newSection = obj->m_section;
        if(newSection == oldSection)
            return;
        if(m_firstVisibleSection <= newSection && newSection <= m_lastVisibleSection)
            return;
        if(!(m_firstVisibleSection <= oldSection && oldSection <= m_lastVisibleSection))
            return;
        {
            std::scoped_lock lock(processedGroupsMutex);
            if(!m_processedGroups->containsObject(obj))
                m_processedGroups->addObject(obj);
        }
        obj->m_shouldHide = true;
    }
};
#endif
