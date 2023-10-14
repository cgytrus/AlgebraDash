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
struct FastMoveActions : geode::Modify<FastMoveActions, GJBaseGameLayer> {
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

                std::vector<GameObject*> objects;
                objects.reserve(b - a);
                std::vector<GameObject*> disabledObjects;
                disabledObjects.reserve(b - a);
                std::vector<GameObject*> reorderObjects;
                reorderObjects.reserve(b - a);

                for(unsigned int i = a; i < b; ++i) {
                    auto* obj = (GameObject*)arr[i];
                    if(!obj)
                        break;
                    moveObject(obj, moveX, moveY, moveObjType, objects, disabledObjects, reorderObjects);
                }

                if (!objects.empty()) {
                    std::unique_lock lock(objectsVecMutex, std::try_to_lock_t());
                    if (lock) {
                        for (const auto& obj : objects)
                            m_objectsVec.push_back(obj);
                        objects.clear();
                    }
                }
                if (!disabledObjects.empty()) {
                    std::unique_lock lock(disabledObjectsMutex, std::try_to_lock_t());
                    if (lock) {
                        for (const auto& obj : disabledObjects)
                            m_disabledObjects.push_back(obj);
                        disabledObjects.clear();
                    }
                }
                if (!reorderObjects.empty()) {
                    std::scoped_lock lock(reorderObjectSectionMutex);
                    for (const auto& obj : reorderObjects)
                        reorderObjectSection(obj);
                }
                if (!objects.empty()) {
                    std::scoped_lock lock(objectsVecMutex);
                    for (const auto& obj : objects)
                        m_objectsVec.push_back(obj);
                }
                if (!disabledObjects.empty()) {
                    std::scoped_lock lock(disabledObjectsMutex);
                    for (const auto& obj : disabledObjects)
                        m_disabledObjects.push_back(obj);
                }
            });
        {
            ZoneScopedN("wait");
            moveJob.wait();
        }
    }
    void moveObject(GameObject* obj, float moveX, float moveY, CCObjectType moveObjType,
        std::vector<GameObject*>& objects, std::vector<GameObject*>& disabledObjects,
        std::vector<GameObject*>& reorderObjects) {
        ZoneScoped;

        if (!obj->m_inOptimizedGroup) {
            if (!obj->m_queuedForPositionUpdate) {
                obj->m_firstPosition = obj->m_startPosition + obj->m_startPosOffset;
                obj->m_queuedForPositionUpdate = true;
                objects.push_back(obj);
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
                reorderObjects.push_back(obj);
        }

        if (obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
            obj->m_queuedForPositionUpdate = true;
            disabledObjects.push_back(obj);
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
        BS::multi_future<void> rotateJob = sharedPool().parallelize_loop(0, objects->data->num,
            [this, arr, rotationDeg, rotationRad, parentPos, lockRot, forceUpdate](unsigned int a, unsigned int b) {
                ZoneScopedN("rotate around job");

                std::vector<GameObject*> objects;
                objects.reserve(b - a);
                std::vector<GameObject*> disabledObjects;
                disabledObjects.reserve(b - a);
                std::vector<GameObject*> reorderObjects;
                reorderObjects.reserve(b - a);

                for(unsigned int i = a; i < b; ++i) {
                    auto* obj = (GameObject*)arr[i];
                    if(!obj)
                        break;
                    obj->m_wasForcedRotatedPositionUpdateIdk = forceUpdate;

                    if(!obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
                        obj->m_firstPosition = obj->getRealPosition();
                        obj->m_queuedForPositionUpdate = true;
                        objects.push_back(obj);
                        obj->m_isObjectRectDirty = true;
                        obj->m_isOrientedRectDirty = true;
                    }
                    obj->m_textureRectDirty = true;

                    rotateObjectPosition(obj, rotationRad, parentPos);
                    if(!lockRot)
                        rotateObject(obj, rotationDeg);
                    if (sectionChanged(obj))
                        reorderObjects.push_back(obj);

                    if(obj->m_inOptimizedGroup && !obj->m_queuedForPositionUpdate) {
                        obj->m_queuedForPositionUpdate = true;
                        disabledObjects.push_back(obj);
                    }
                }

                if (!objects.empty()) {
                    std::unique_lock lock(objectsVecMutex, std::try_to_lock_t());
                    if (lock) {
                        for (const auto& obj : objects)
                            m_objectsVec.push_back(obj);
                        objects.clear();
                    }
                }
                if (!disabledObjects.empty()) {
                    std::unique_lock lock(disabledObjectsMutex, std::try_to_lock_t());
                    if (lock) {
                        for (const auto& obj : disabledObjects)
                            m_disabledObjects.push_back(obj);
                        disabledObjects.clear();
                    }
                }
                if (!reorderObjects.empty()) {
                    std::scoped_lock lock(reorderObjectSectionMutex);
                    for (const auto& obj : reorderObjects)
                        reorderObjectSection(obj);
                }
                if (!objects.empty()) {
                    std::scoped_lock lock(objectsVecMutex);
                    for (const auto& obj : objects)
                        m_objectsVec.push_back(obj);
                }
                if (!disabledObjects.empty()) {
                    std::scoped_lock lock(disabledObjectsMutex);
                    for (const auto& obj : disabledObjects)
                        m_disabledObjects.push_back(obj);
                }
            });
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
