#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <numbers>
#include <shared_pool.hpp>

#ifdef GEODE_IS_WINDOWS
std::vector<std::vector<GameObject*>*> objectsVecVec;
std::vector<std::vector<GameObject*>*> disabledObjectsVec;
std::vector<std::vector<GameObject*>*> reorderObjectsVec;
struct FastMoveActions : geode::Modify<FastMoveActions, GJBaseGameLayer> {
    void processMoveActionsStep(float dt) {
        ZoneScoped;

        m_objectsVec.clear();
        m_disabledObjects.clear();

        for (auto& vec : objectsVecVec)
            vec->clear();
        for (auto& vec : disabledObjectsVec)
            vec->clear();
        for (auto& vec : reorderObjectsVec)
            vec->clear();

        processRotationActions();
        processMoveActions();
        processPlayerFollowActions(dt);
        processFollowActions();

        for (const auto& reorderObjects : reorderObjectsVec)
            for (const auto& obj : *reorderObjects)
                reorderObjectSection(obj);

        for (const auto& obj : m_objectsVec) {
            obj->m_queuedForPositionUpdate = false;
            if (m_objectsAreDisabled)
                obj->quickUpdatePosition();
            if (obj->m_shouldUpdateFirstPosition) {
                obj->m_shouldUpdateFirstPosition = false;
                obj->m_firstPosition = obj->getRealPosition();
            }
        }
        for (const auto& objectsVec : objectsVecVec) {
            for (const auto& obj : *objectsVec) {
                obj->m_queuedForPositionUpdate = false;
                if (m_objectsAreDisabled)
                    obj->quickUpdatePosition();
                if (obj->m_shouldUpdateFirstPosition) {
                    obj->m_shouldUpdateFirstPosition = false;
                    obj->m_firstPosition = obj->getRealPosition();
                }
            }
        }

        if (m_objectsAreDisabled) {
            for (const auto& obj : m_disabledObjects) {
                obj->m_queuedForPositionUpdate = false;
                if (m_objectsAreDisabled)
                    obj->quickUpdatePosition();
            }
            for (const auto& disabledObjects : disabledObjectsVec) {
                for (const auto& obj : *disabledObjects) {
                    obj->m_queuedForPositionUpdate = false;
                    if (m_objectsAreDisabled)
                        obj->quickUpdatePosition();
                }
            }
        }
    }

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
        auto& pool = sharedPool();
        BS::blocks blks(0, objects->data->num, pool.get_thread_count());
        if (blks.get_total_size() <= 0)
            return;
        BS::multi_future<void> moveJob(blks.get_num_blocks());
        while (objectsVecVec.size() < blks.get_num_blocks())
            objectsVecVec.push_back(new std::vector<GameObject*>());
        while (disabledObjectsVec.size() < blks.get_num_blocks())
            disabledObjectsVec.push_back(new std::vector<GameObject*>());
        while (reorderObjectsVec.size() < blks.get_num_blocks())
            reorderObjectsVec.push_back(new std::vector<GameObject*>());
        for (size_t i = 0; i < blks.get_num_blocks(); ++i) {
            auto* objectsVec = objectsVecVec[i];
            auto* disabledObjects = disabledObjectsVec[i];
            auto* reorderObjects = reorderObjectsVec[i];
            moveJob[i] = pool.submit(
                [=, this](unsigned int a, unsigned int b) {
                    ZoneScopedN("move job");
                    for (unsigned int i = a; i < b; ++i) {
                        auto* obj = (GameObject*)arr[i];
                        if (!obj)
                            break;
                        if (!obj->m_inOptimizedGroup) {
                            if (!obj->m_queuedForPositionUpdate) {
                                obj->m_firstPosition = obj->m_startPosition + obj->m_startPosOffset;
                                obj->m_queuedForPositionUpdate = true;
                                objectsVec->push_back(obj);
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
                            if (sectionChanged(obj))
                                reorderObjects->push_back(obj);
                        }

                        if (obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
                            obj->m_queuedForPositionUpdate = true;
                            disabledObjects->push_back(obj);
                        }
                    }
                }, blks.start(i), blks.end(i));
        }
        {
            ZoneScopedN("wait");
            moveJob.wait();
        }
    }

    void processRotationActions() {
        ZoneScoped;

        CCDictElement* el;
        auto dict = m_effectManager->m_rotationNodes;
        CCDICT_FOREACH(dict, el) {
                ZoneScopedN("m_rotationNodes iteration");
                auto moveNode = (CCMoveCNode*)el->getObject();
                int moveTag = moveNode->getTag();

                int parentGroupId = std::min(moveNode->m_parentGroupId, 999);
                auto parentGroup = m_groups[parentGroupId];
                if(!parentGroup) {
                    parentGroup = CCArray::create();
                    m_groupDict->setObject(parentGroup, parentGroupId);
                    m_groups[parentGroupId] = parentGroup;
                }

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

                processRotationsInner(staticObjects, parentGroup, moveNode->m_staticRotation, moveNode);
                processRotationsInner(optimizedObjects, parentGroup, moveNode->m_optimizedRotation, moveNode);
            }
    }
    void processRotationsInner(CCArray* objects, CCArray* parentGroup, float rotationDeg, CCMoveCNode* moveNode) {
        ZoneScoped;
        if(!moveNode->m_forceUpdate && rotationDeg == 0.f)
            return;
        if(parentGroup->count() == 1) {
            auto parentObj = reinterpret_cast<GameObject*>(parentGroup->objectAtIndex(0));
            processRotationAround(objects, parentObj->getRealPosition(), rotationDeg, moveNode->m_forceUpdate, moveNode->m_lockObjectRotation);
        }
        else if(!moveNode->m_lockObjectRotation)
            processSimpleRotation(objects, rotationDeg);
    }
    void processSimpleRotation(CCArray* objects, float rotationDeg) {
        ZoneScoped;
        if(objects->count() == 0)
            return;
        CCObject** arr = objects->data->arr;
        BS::multi_future<void> rotateJob = sharedPool().parallelize_loop(0, objects->data->num,
            [this, arr, rotationDeg](unsigned int a, unsigned int b) {
                ZoneScopedN("rotate simple job");
                for(unsigned int i = a; i < b; ++i) {
                    GameObject* obj = (GameObject*)arr[i];
                    if(!obj)
                        break;
                    rotateObject(obj, rotationDeg);
                }
            });
        {
            ZoneScopedN("wait");
            rotateJob.wait();
        }
    }
    void processRotationAround(CCArray* objects, CCPoint parentPos, float rotationDeg, bool forceUpdate, bool lockRot) {
        ZoneScoped;
        float rotationRad = -rotationDeg / 180.f * std::numbers::pi_v<float>;
        if(objects->count() == 0)
            return;
        CCObject** arr = objects->data->arr;
        auto& pool = sharedPool();
        BS::blocks blks(0, objects->data->num, pool.get_thread_count());
        if (blks.get_total_size() <= 0)
            return;
        BS::multi_future<void> rotateJob(blks.get_num_blocks());
        while (objectsVecVec.size() < blks.get_num_blocks())
            objectsVecVec.push_back(new std::vector<GameObject*>());
        while (disabledObjectsVec.size() < blks.get_num_blocks())
            disabledObjectsVec.push_back(new std::vector<GameObject*>());
        while (reorderObjectsVec.size() < blks.get_num_blocks())
            reorderObjectsVec.push_back(new std::vector<GameObject*>());
        for (size_t i = 0; i < blks.get_num_blocks(); ++i) {
            auto* objectsVec = objectsVecVec[i];
            auto* disabledObjects = disabledObjectsVec[i];
            auto* reorderObjects = reorderObjectsVec[i];
            rotateJob[i] = pool.submit(
                [=, this](unsigned int a, unsigned int b) {
                    ZoneScopedN("rotate around job");
                    for(unsigned int i = a; i < b; ++i) {
                        auto* obj = (GameObject*)arr[i];
                        if(!obj)
                            break;
                        obj->m_wasForcedRotatedPositionUpdateIdk = forceUpdate;

                        if(!obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
                            obj->m_firstPosition = obj->getRealPosition();
                            obj->m_queuedForPositionUpdate = true;
                            objectsVec->push_back(obj);
                            obj->m_isObjectRectDirty = true;
                            obj->m_isOrientedRectDirty = true;
                        }
                        obj->m_textureRectDirty = true;

                        rotateObjectPosition(obj, rotationRad, parentPos);
                        if(!lockRot)
                            rotateObject(obj, rotationDeg);
                        if (sectionChanged(obj))
                            reorderObjects->push_back(obj);

                        if(obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
                            obj->m_queuedForPositionUpdate = true;
                            disabledObjects->push_back(obj);
                        }
                    }
                }, blks.start(i), blks.end(i));
        }
        {
            ZoneScopedN("wait");
            rotateJob.wait();
        }
    }
    void rotateObject(GameObject* obj, float rotationDeg) {
        ZoneScoped;
        if(!obj->m_hasOrientedBox)
            return;
        obj->m_rotateOffset += rotationDeg;
        obj->setRotation(obj->getRotation() + rotationDeg);
        if(obj->m_objectType == GameObjectType::Decoration)
            return;
        obj->m_isObjectRectDirty = true;
        obj->m_isOrientedRectDirty = true;
        if(obj->m_oriented)
            return;
        obj->m_oriented = true;
        obj->updateOrientedBox();
        obj->getObjectRect();
    }
    void rotateObjectPosition(GameObject* obj, float rotationRad, CCPoint around) {
        ZoneScoped;
        CCPoint pos = obj->getRealPosition();
        CCPoint relativePos = pos - around;
        float newRotationRad = std::atan2f(relativePos.y, relativePos.x) + rotationRad;
        CCPoint newOffset{std::cosf(newRotationRad), std::sinf(newRotationRad)};
        newOffset *= std::sqrt(relativePos.x * relativePos.x + relativePos.y * relativePos.y);
        obj->m_startPosOffset += around + newOffset - pos;
    }

    bool sectionChanged(GameObject* obj) {
        return std::max((int)((obj->m_startPosition.x + obj->m_startPosOffset.x) / 100.f), 0) != obj->m_section;
    }
};
#endif
