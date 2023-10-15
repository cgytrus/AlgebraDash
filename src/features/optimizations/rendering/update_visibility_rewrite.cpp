#include <Tracy.hpp>
#include <numbers>
#include <shared_pool.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>

#ifdef GEODE_IS_WINDOWS
static float getMeteringValue(GameSoundManager* self) {
    ZoneScoped;
    reinterpret_cast<void(__cdecl*)(GameSoundManager*)>(base::get() + 0x258f0)(self);
    float res;
    __asm movss res, xmm0;
    return res;
}

static cocos2d::ccColor3B getMixedColor(cocos2d::ccColor3B top, cocos2d::ccColor3B bottom, float t) {
    ZoneScoped;
    cocos2d::ccColor3B ret;
    __asm movss xmm3, t;
    reinterpret_cast<void(__cdecl*)(cocos2d::ccColor3B*, cocos2d::ccColor3B, cocos2d::ccColor3B)>(base::get() + 0x11f610)(&ret, top, bottom);
    return ret;
}
static cocos2d::ccColor3B transformColor(cocos2d::ccColor3B const& src, cocos2d::ccHSVValue hsv) {
    ZoneScoped;
    cocos2d::ccColor3B ret;
    reinterpret_cast<void(__thiscall*)(cocos2d::ccColor3B const&, cocos2d::ccColor3B*, cocos2d::ccHSVValue)>(base::get() + 0x26a60)(src, &ret, hsv);
    __asm add esp, 0x14
    return ret;
}

static inline cocos2d::ccColor3B transformColor(ccColor3B const& src, float h, float s, float v) {
    //cocos2d::ccColor3B dest;
    //GameToolbox::transformColor(src, nullptr, dest, cocos2d::cchsv(h, s, v, true, true));
    return transformColor(src, cocos2d::cchsv(h, s, v, true, true));
}

/*struct NewGameObject : geode::Modify<NewGameObject, GameObject> {
    bool m_baseColorCached = false;
    bool m_detailColorCached = false;
    ccColor3B m_cachedBaseColor;
    ccColor3B m_cachedDetailColor;

    ccColor3B& colorForMode(int colorMode, bool isMain) {
        ZoneScoped;

        if (isMain && m_fields->m_baseColorCached)
            return m_fields->m_cachedBaseColor;
        if (!isMain && m_fields->m_detailColorCached)
            return m_fields->m_cachedDetailColor;

        m_color = colorMode < 1 ?
            ccc3(255, 255, 255) :
            getActiveColorForMode(colorMode, isMain);

        if (m_pulseGroupCount > 0) {
            if (isMain)
                m_color = groupColor(m_color, !m_unk36A);
            else if (m_detailColorID != 1012)
                m_color = groupColor(m_color, false);
        }

        if (isMain) {
            if (m_baseColorHSVModified)
                m_color = transformColor(m_color, m_baseColor->m_hsv);
            if (m_pulseGroupCount <= 0) {
                m_fields->m_baseColorCached = true;
                m_fields->m_cachedBaseColor = m_color;
            }
        }
        else {
            if (m_detailColorHSVModified)
                m_color = transformColor(m_color, m_detailColor->m_hsv);
            if (m_pulseGroupCount <= 0) {
                m_fields->m_detailColorCached = true;
                m_fields->m_cachedDetailColor = m_color;
            }
        }

        return m_color;
    }
};*/

float groupOpacityMod(GameObject* self) {
    ZoneScoped;
    reinterpret_cast<void(__thiscall*)(GameObject*)>(base::get() + 0xebda0)(self);
    float x;
    __asm movss x, xmm0;
    return x;
}
float opacityModForMode(GameObject* self, int mode, bool isMain) {
    ZoneScoped;

    float res = 1.f;
    if(mode > 0) {
        auto colorActionSprite = isMain ? self->m_colorActionSpriteBase : self->m_colorActionSpriteDetail;
        auto opacity = colorActionSprite->m_opacity;
        if(opacity < 250.f)
            res = opacity * 0.004f;
    }
    if(self->m_alphaGroupCount > 0)
        res *= groupOpacityMod(self);
    return res;
}

void updateMainColor(GameObject* self) {
    ZoneScoped;

    auto colorMode = self->m_baseColorID;
    if(colorMode == 0 && self->m_pulseGroupCount == 0 && self->m_alphaGroupCount == 0)
        return;
    auto color = self->colorForMode(colorMode, true);
    self->setObjectColor(color);
    self->m_baseColor->m_opacity = opacityModForMode(self, colorMode, true);
}

void updateSecondaryColor(GameObject* self) {
    ZoneScoped;

    bool hasSecondaryColor = self->m_detailSprite != nullptr;
    auto colorMode = self->m_detailColorID;
    if(!hasSecondaryColor || colorMode == 0 && self->m_groupCount == 0)
        return;
    auto color = self->colorForMode(colorMode, false);
    self->setChildColor(color);
    self->m_detailColor->m_opacity = opacityModForMode(self, colorMode, false);
}

void setGlowOpacity(GameObject* self, GLubyte opacity) {
    ZoneScoped;
    if(self->m_glowSprite == nullptr)
        return;
    self->m_glowSprite->setOpacity(opacity);
    self->m_glowSprite->setChildOpacity(opacity);
}

float* s_halfScreenWidth;
$execute {
    s_halfScreenWidth = reinterpret_cast<float*>(base::get() + 0x3222f4);
}

struct update_game_object_data_t {
    float meteringValue;

    ccColor3B transCol0;

    ccColor3B lightBgColor;

    ccColor3B bgColor;
    ccColor3B transformedBgColor;
    bool transBgColorIsDark;

    float screenLeft;
    float screenRight;

    float playerPos;
    float boundLeft;
    float boundRight;
    float offsetBoundRight;
    float cameraCenterX;
    CCPoint cameraPosition;

    float totalTime;
    int activeEnterEffect;

    bool isDead;

    float mirrorTransition;
    bool unk460;
    float cameraFlip;
    bool unk3EC;
};

float getRelativeModxd(update_game_object_data_t& data, float objX, float outSpeed, float inSpeed) {
    ZoneScoped;
    return std::clamp(objX <= data.cameraCenterX ? (objX - data.cameraPosition.x) * inSpeed :
        (data.cameraCenterX + *s_halfScreenWidth - objX) * outSpeed, 0.f, 1.f);
}

void applyEnterEffect(update_game_object_data_t& data, GameObject& gameObject) {
    ZoneScoped;
    CCPoint realPos = gameObject.getRealPosition();
    float relativeMod = getRelativeModxd(data, realPos.x, 0.017f, 0.017f);
    bool shouldStop = relativeMod >= 1.f;
    if(gameObject.m_ignoreEnter) {
        shouldStop = true;
    }

    if(gameObject.m_activeEnterEffect == 0) {
        if(shouldStop) {
            auto pos = gameObject.getPosition();
            if (realPos.x == pos.x && realPos.y == pos.y)
                return;
            gameObject.setPosition(realPos);
            return;
        }
        float cameraCenterY = data.cameraPosition.y + 90.f;
        gameObject.m_activeEnterEffect = data.activeEnterEffect;
        switch(data.activeEnterEffect) {
            case 8:
                gameObject.m_enterAngle = realPos.y >= cameraCenterY ? 45.f : 135.f;
                break;
            case 9:
                gameObject.m_enterAngle = realPos.y >= cameraCenterY ? -45.f : -135.f;
                break;
            case 10:
                gameObject.m_enterAngle = ((float)rand() / 32767.f * 2.f - 1.f) * 180.f;
                break;
            case 11:
                gameObject.m_enterAngle = realPos.y >= cameraCenterY ? 0.f : 180.f;
                break;
            case 12:
                gameObject.m_enterAngle = realPos.y >= cameraCenterY ? 180.f : 0.f;
                break;
        }
    }
    else if(shouldStop)
        gameObject.m_activeEnterEffect = 0;

    float scale = 1.f;
    switch(gameObject.m_activeEnterEffect) {
        case 2:
            scale = relativeMod;
            break;
        case 3:
            scale = (1.f - relativeMod) * 0.75f + 1.f;
            break;
        case 4:
            realPos -= ccp(0.f, (1.f - relativeMod) * 100.f);
            break;
        case 5:
            realPos -= ccp(0.f, (1.f - relativeMod) * -100.f);
            break;
        case 6:
            realPos -= ccp((1.f - relativeMod) * -100.f, 0.f);
            break;
        case 7:
            realPos -= ccp((1.f - relativeMod) * 100.f, 0.f);
            break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            realPos -=
                ccpForAngle((gameObject.m_enterAngle - 90.f) * std::numbers::pi_v<float> / 180.f) *
                (1.f - relativeMod) * 100.f;
            break;
    }
    if (!gameObject.m_useAudioScale) {
        gameObject.setRScaleX(gameObject.m_obStartScale.width * scale);
        gameObject.setRScaleY(gameObject.m_obStartScale.height * scale);
    }
    gameObject.setPosition(realPos);
    if(relativeMod == 0.f)
        gameObject.m_activeEnterEffect = 0;
}

void preUpdateGameObject(GameObject& gameObject, gd::map<short, bool>& hasColors) {
    if (gameObject.m_active && !hasColors.empty()) {
        bool hasBaseColor = hasColors[(short)gameObject.m_baseColorID];
        bool hasDetailColor = gameObject.m_detailSprite != nullptr &&
            hasColors[(short)gameObject.m_detailColorID];
        if (hasBaseColor || hasDetailColor) {
            gameObject.addMainSpriteToParent(false);
            gameObject.addColorSpriteToParent(false);
        }
    }

    gameObject.activateObject();
}
void updateGameObject(GameObject& gameObject, update_game_object_data_t data) {
    ZoneScoped;

    updateMainColor(&gameObject);
    updateSecondaryColor(&gameObject);

    if(gameObject.m_isAnimated)
        gameObject.updateSyncedAnimation(data.totalTime);

    CCPoint objPos = gameObject.getRealPosition();
    if(objPos.x <= data.cameraCenterX)
        objPos.x += gameObject.m_rectXCenterMaybe;
    else
        objPos.x -= gameObject.m_rectXCenterMaybe;
    float relativeMod = getRelativeModxd(data, objPos.x, 0.014285714f, 0.014285714f);

    // idk
    if(*(bool*)((uintptr_t)&gameObject.m_baseSprite + 2))
        typeinfo_cast<AnimatedGameObject*>(&gameObject)->updateChildSpriteColor(data.transCol0);

    // breakable block
    if(gameObject.m_objectID == 143)
        gameObject.setGlowColor(data.transformedBgColor);

    // idk
    if(gameObject.m_unk364)
        return;

    // pulse
    if(gameObject.m_useAudioScale)
        gameObject.setRScale(gameObject.m_customAudioScale ?
            std::lerp(gameObject.m_minAudioScale, gameObject.m_maxAudioScale, data.meteringValue - 0.1f) :
            data.meteringValue);

    // fade/enter
    bool ignoreFade = gameObject.m_ignoreFade;
    bool unk365 = gameObject.m_unk365;
    bool unk368 = gameObject.m_unk368;
    bool unk3b0 = gameObject.m_unk3b0;
    int objActiveEnterEffect = gameObject.m_activeEnterEffect;
    int activeEnterEffect = data.activeEnterEffect;
    gameObject.setOpacity(!ignoreFade && (((!unk365 || unk368 && unk3b0) || objActiveEnterEffect != 0 && objActiveEnterEffect != 1) || activeEnterEffect != 1) ?
        (GLubyte)std::round(relativeMod * 255.f) : 255u);

    // idk
    if(gameObject.m_glowUserBackgroundColour)
        gameObject.setGlowColor(gameObject.m_useSpecialLight ? data.bgColor : data.transformedBgColor);

    // invisible objects
    if(gameObject.m_invisibleMode) {
        float invisibleRelativeMod = getRelativeModxd(data, objPos.x, 0.02f, 0.014285714f);
        if(data.isDead)
            gameObject.setGlowColor(data.transformedBgColor);
        else {
            float valIdk = std::clamp(data.cameraPosition.x + data.boundRight - objPos.x > 0.f ?
                (data.cameraPosition.x + data.playerPos - objPos.x) / std::max(data.boundLeft, 1.f) :
                (objPos.x - data.cameraPosition.x - data.boundRight) / std::max(data.offsetBoundRight, 1.f), 0.f, 1.f);
            auto mainOpacity = std::min((0.05f + valIdk * 0.95f), invisibleRelativeMod);
            auto glowOpacity = std::min((0.15f + valIdk * 0.85f), invisibleRelativeMod);
            gameObject.setOpacity((GLubyte)(mainOpacity * 255.f));
            setGlowOpacity(&gameObject, (GLubyte)(glowOpacity * 255.f));

            if(mainOpacity <= 0.8f)
                gameObject.setGlowColor(data.transformedBgColor);
            else {
                auto useCol = data.transBgColorIsDark ? ccColor3B{255u, 255u, 255u} : data.lightBgColor;
                float t = 0.7f + (1.f - (mainOpacity - 0.8f) / 0.2f) * 0.3f;
                gameObject.setGlowColor(getMixedColor(data.transformedBgColor, useCol, t));
            }
        }
    }

    applyEnterEffect(data, gameObject);

    bool isFlipping = data.mirrorTransition != 0.f && data.mirrorTransition != 1.f;
    if(!isFlipping) {
        if(!data.unk460)
            return;
        gameObject.setFlipX(gameObject.m_startFlipX);
        gameObject.setFlipY(gameObject.m_startFlipY);
        gameObject.setRotation(gameObject.m_rotation);
        gameObject.setPosition(gameObject.getPosition());
        return;
    }

    // mirror animation
    float t = data.cameraFlip == -1.f ? 1.f - data.mirrorTransition : data.mirrorTransition;
    auto pos = gameObject.getPosition();
    pos.x += ((data.screenLeft + data.screenRight) - (pos.x - data.cameraPosition.x) * 2.f) * t;
    gameObject.setPosition(pos);
    float rot = std::abs(gameObject.getRotation());
    bool isOnSide = rot == 90.f || rot == 270.f;
    if(data.cameraFlip == 1.f && data.mirrorTransition >= 0.5f || data.cameraFlip != 1.f && data.mirrorTransition <= 0.5f) {
        if(isOnSide)
            gameObject.setFlipY(!gameObject.m_startFlipY);
        else
            gameObject.setFlipX(!gameObject.m_startFlipX);
        gameObject.setRotation(-gameObject.m_rotation);
    }
    else if(data.unk3EC && (data.cameraFlip != 1.f && data.mirrorTransition >= 0.5f ||
        data.cameraFlip == 1.f && data.mirrorTransition <= 0.5f)) {
        if(isOnSide)
            gameObject.setFlipY(gameObject.m_startFlipY);
        else
            gameObject.setFlipX(gameObject.m_startFlipX);
        gameObject.setRotation(data.cameraFlip == 1.f ? gameObject.m_rotation : -gameObject.m_rotation);
    }
}

//float s_lastSwitch = 0.f;
//bool s_switchxd = false;
struct NewPlayLayer : geode::Modify<NewPlayLayer, PlayLayer> {
    void updateVisibility() {
        ZoneNamed(___tracy_scoped_zone_full, true);

        //if(std::abs(s_lastSwitch - m_totalTime) >= 0.5f) {
        //    s_lastSwitch = m_totalTime;
        //    s_switchxd = !s_switchxd;
        //    log::debug("switched to {}", s_switchxd ? "vanilla" : "decomp");
        //}
        //if(s_switchxd) {
        //    PlayLayer::updateVisibility();
        //    return;
        //}

        m_effectManager->m_time = m_totalTime;

        m_effectManager->processColors();

        auto player1Color = m_effectManager->activeColorForIndex(1005);
        m_effectManager->calculateLightBGColor(player1Color);

        update_game_object_data_t data;

        data.meteringValue = m_isPracticeMode || m_isMute ? 0.5f :
            m_isAudioMeteringSupported ? getMeteringValue(GameSoundManager::sharedManager()) :
                m_audioEffectsLayer->m_unk1B0;

        m_player1->m_meteringValue = data.meteringValue;
        m_player2->m_meteringValue = data.meteringValue;

        auto sectionObjects = CCArrayExt<CCArray*>(m_sectionObjects);
        int sectionCount = (int)sectionObjects.size();

        auto director = CCDirector::sharedDirector();
        CCPoint winSize = director->getWinSize();
        int startSection = std::clamp((int)std::floor(m_cameraPosition.x / 100.f) - 1, 0, sectionCount - 1);
        int endSection = std::clamp((int)std::ceil((m_cameraPosition.x + winSize.x) / 100.f) + 1, 0, sectionCount - 1);

        int lastInvisibleSectionLeft =
            m_lastVisibleSection < startSection ? m_lastVisibleSection : startSection - 1;
        int firstInvisibleSectionRight =
            m_firstVisibleSection > endSection ? m_firstVisibleSection : endSection + 1;

        CCRect camRect(m_cameraPosition.x, m_cameraPosition.y, winSize.x, winSize.y);
        float camLeft = camRect.getMinX();
        float camRight = camRect.getMaxX();
        float camBottom = camRect.getMinY();
        float camTop = camRect.getMaxY();
        data.cameraPosition = m_cameraPosition;
        data.cameraCenterX = m_cameraPosition.x + *s_halfScreenWidth;

        data.transCol0 = transformColor(unk348->getColor(), 0.f, -0.3f, 0.4f);

        data.lightBgColor = m_effectManager->activeColorForIndex(1007);

        data.bgColor = m_effectManager->activeColorForIndex(1000);
        data.transformedBgColor = transformColor(data.bgColor, 0.f, -0.2f, 0.2f);
        data.transBgColorIsDark = data.transformedBgColor.r + data.transformedBgColor.g + data.transformedBgColor.b < 150u;

        data.screenLeft = director->getScreenLeft();
        data.screenRight = director->getScreenRight();

        data.playerPos = winSize.x * 0.5f - 75.f;
        data.boundLeft = data.playerPos - 30.f;
        data.boundRight = data.playerPos + 110.f;
        data.offsetBoundRight = data.screenRight - data.boundRight - 90.f;

        data.totalTime = m_totalTime;
        data.activeEnterEffect = m_activeEnterEffect;

        data.isDead = m_player1->m_isDead;

        data.mirrorTransition = m_mirrorTransition;
        data.unk460 = unk460;
        data.cameraFlip = m_cameraFlip;
        data.unk3EC = unk3EC;

        {
            ZoneScopedN("loop 0: hide previously visible objects left");
            for (auto i = m_firstVisibleSection; i <= lastInvisibleSectionLeft; i++) {
                for (const auto& gameObject : CCArrayExt<GameObject*>(sectionObjects[i])) {
                    gameObject->m_shouldHide = true;
                }
            }
        }
        {
            ZoneScopedN("loop 0.5: hide previously visible objects right");
            for (auto i = firstInvisibleSectionRight; i <= m_lastVisibleSection; i++) {
                for (const auto& gameObject : CCArrayExt<GameObject*>(sectionObjects[i])) {
                    gameObject->m_shouldHide = true;
                }
            }
        }

        {
            ZoneScopedN("loop 1: queue objects for update");
            for(auto i = startSection; i <= endSection; i++) {
                bool isOnEdge = i <= startSection + 1 || i >= endSection - 1;
                for (const auto& gameObject : CCArrayExt<GameObject*>(sectionObjects[i])) {
                    if(gameObject->m_groupDisabled) {
                        gameObject->m_shouldHide = true;
                        if (!m_fullReset)
                            gameObject->deactivateObject(false);
                        continue;
                    }
                    if(gameObject->m_active && !isOnEdge) {
                        gameObject->m_shouldHide = false;
                        m_objectsToUpdate->addObject(gameObject);
                        preUpdateGameObject(*gameObject, m_hasColors);
                        continue;
                    }
                    auto rect = gameObject->getObjectTextureRect();
                    if(!gameObject->m_unk2e8 &&
                        (rect.getMinX() > camRight || rect.getMaxX() < camLeft ||
                        rect.getMinY() > camTop || rect.getMaxY() < camBottom)) {
                        gameObject->m_shouldHide = true;
                        if (!m_fullReset)
                            gameObject->deactivateObject(false);
                        continue;
                    }
                    gameObject->m_shouldHide = false;
                    m_objectsToUpdate->addObject(gameObject);
                    preUpdateGameObject(*gameObject, m_hasColors);
                }
            }
        }

        if(!m_fullReset) {
            {
                ZoneScopedN("loop 2: deactivate previously visible objects left");
                for (auto i = m_firstVisibleSection; i <= lastInvisibleSectionLeft; i++) {
                    for (const auto& gameObject : CCArrayExt<GameObject*>(sectionObjects[i]))
                        gameObject->deactivateObject(false);
                }
            }
            {
                ZoneScopedN("loop 2.5: deactivate previously visible objects right");
                for (auto i = firstInvisibleSectionRight; i <= m_lastVisibleSection; i++) {
                    for (const auto& gameObject : CCArrayExt<GameObject*>(sectionObjects[i]))
                        gameObject->deactivateObject(false);
                }
            }
        }

        //{
        //    ZoneScopedN("loop 3: deactivate invisible objects after moving");
        //    for (const auto& gameObject : CCArrayExt<GameObject*>(m_processedGroups)) {
        //        if(gameObject->m_section < startSection || gameObject->m_section > endSection)
        //            gameObject->deactivateObject(true);
        //    }
            m_processedGroups->removeAllObjects();
        //}

        {
            ZoneScopedN("loop 4: update objects");
            CCObject** objects = m_objectsToUpdate->data->arr;
            BS::multi_future<void> updateObjectsJob =
                sharedPool().parallelize_loop(m_objectsToUpdate->data->num,
                    [=](size_t a, size_t b) {
                        ZoneNamedN(___tracy_scoped_zone_job, "update objects job", true);
                        for (size_t i = a; i < b; ++i) {
                            updateGameObject(*reinterpret_cast<GameObject*>(objects[i]), data);
                        }
                    });
            {
                ZoneNamedN(___tracy_scoped_zone_job_wait, "wait", true);
                updateObjectsJob.wait();
            }
            //for (const auto& gameObject : CCArrayExt<GameObject*>(m_objectsToUpdate)) {
            //    updateGameObject(*gameObject, data);
            //}
            m_objectsToUpdate->removeAllObjects();
        }

        m_firstVisibleSection = startSection;
        m_lastVisibleSection = endSection;
        m_fullReset = false;

        unk460 = false;
        unk464->removeAllObjects();
        m_hasColors.clear();
    }
};
#endif
