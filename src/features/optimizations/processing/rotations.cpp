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
struct FastRotations : geode::Modify<FastRotations, GJBaseGameLayer> {
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
                for(unsigned int i = a; i < b; ++i) {
                    GameObject* obj = (GameObject*)arr[i];
                    if(!obj)
                        break;
                    obj->m_wasForcedRotatedPositionUpdateIdk = forceUpdate;
                    if(!obj->m_unk45A && !obj->m_unk42C) {
                        obj->m_firstPosition = obj->getRealPosition();
                        obj->m_unk42C = true;
                        {
                            std::unique_lock lock(objectsVecMutex);
                            m_objectsVec.push_back(obj);
                        }
                        obj->m_isObjectRectDirty = true;
                        obj->m_isOrientedRectDirty = true;
                    }
                    obj->m_textureRectDirty = true;
                    rotateObjectPosition(obj, rotationRad, parentPos);
                    if(!lockRot)
                        rotateObject(obj, rotationDeg);
                    checkSectionChange(obj);
                    if(obj->m_unk45A && !obj->m_unk42C) {
                        obj->m_unk42C = true;
                        {
                            std::unique_lock lock(disabledObjectsMutex);
                            m_disabledObjects.push_back(obj);
                        }
                    }
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

    void checkSectionChange(GameObject* obj) {
        ZoneScoped;

        int oldSection = obj->m_section;
        int newSection = std::max((int)((obj->m_startPosition.x + obj->m_startPosOffset.x) / 100.f), 0);
        if(newSection == oldSection)
            return;
        {
            std::unique_lock lock(reorderObjectSectionMutex);
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
            std::unique_lock lock(processedGroupsMutex);
            if(!m_processedGroups->containsObject(obj))
                m_processedGroups->addObject(obj);
        }
        obj->m_shouldHide = true;
    }
};
#endif
