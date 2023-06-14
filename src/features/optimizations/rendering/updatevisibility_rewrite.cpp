#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/PlayLayer.hpp>

#ifdef GEODE_IS_WINDOWS
static float getMeteringValue(GameSoundManager* self) {
    reinterpret_cast<void(__cdecl*)(GameSoundManager*)>(base::get() + 0x258f0)(self);
    float res;
    __asm movss res, xmm0;
    return res;
}

static cocos2d::ccColor3B getMixedColor(cocos2d::ccColor3B top, cocos2d::ccColor3B bottom, float t) {
    cocos2d::ccColor3B ret;
    __asm movss xmm3, t;
    reinterpret_cast<void(__cdecl*)(cocos2d::ccColor3B*, cocos2d::ccColor3B, cocos2d::ccColor3B)>(base::get() + 0x11f610)(&ret, top, bottom);
    return ret;
}
static cocos2d::ccColor3B transformColor(cocos2d::ccColor3B const& src, cocos2d::ccHSVValue hsv) {
    cocos2d::ccColor3B ret;
    reinterpret_cast<void(__thiscall*)(cocos2d::ccColor3B const&, cocos2d::ccColor3B*, cocos2d::ccHSVValue)>(base::get() + 0x26a60)(src, &ret, hsv);
    __asm add esp, 0x14
    return ret;
}

static inline cocos2d::ccColor3B transformColor(ccColor3B const& src, float h, float s, float v) {
    //cocos2d::ccColor3B dest;
    //GameToolbox::transformColor(src, nullptr, dest, cocos2d::cchsv(h, s, v, true, true));
    return transformColor(src, cocos2d::cchsv(h, s, v, true, true));;
}

float groupOpacityMod(GameObject* self) {
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
    self->m_baseColor->m_unk_0F4 = opacityModForMode(self, colorMode, true);
}

void updateSecondaryColor(GameObject* self) {
    ZoneScoped;

    bool hasSecondaryColor = self->m_detailSprite != nullptr;
    auto colorMode = self->m_detailColorID;
    if(!hasSecondaryColor || colorMode == 0 && self->m_groupCount == 0)
        return;
    auto color = self->colorForMode(colorMode, false);
    self->setChildColor(color);
    self->m_detailColor->m_unk_0F4 = opacityModForMode(self, colorMode, false);
}

void setGlowOpacity(GameObject* self, GLubyte opacity) {
    if(self->m_glowSprite == nullptr)
        return;
    self->m_glowSprite->setOpacity(opacity);
    self->m_glowSprite->setChildOpacity(opacity);
}

float* s_halfScreenWidth;
$execute {
    s_halfScreenWidth = reinterpret_cast<float*>(base::get() + 0x3222f4);
}

float getRelativeModxd(PlayLayer* self, float objX, float outSpeed, float inSpeed) {
    ZoneScoped;

    float cameraCenterX = self->m_cameraPosition.x + *s_halfScreenWidth;
    return std::clamp(objX <= cameraCenterX ? (objX - self->m_cameraPosition.x) * inSpeed :
        (cameraCenterX + *s_halfScreenWidth - objX) * outSpeed, 0.f, 1.f);
}

//float s_lastSwitch = 0.f;
//bool s_switchxd = false;
struct NewPlayLayer : geode::Modify<NewPlayLayer, PlayLayer> {
    void updateVisibility() {
        ZoneScoped;

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

        float meteringValue = m_isPracticeMode || m_isMute ? 0.5f :
            m_isAudioMeteringSupported ? getMeteringValue(GameSoundManager::sharedManager()) :
                m_audioEffectsLayer->m_unk1B0;

        m_player1->m_meteringValue = meteringValue;
        m_player2->m_meteringValue = meteringValue;

        auto director = CCDirector::sharedDirector();
        CCPoint winSize = director->getWinSize();
        int startSection = (int)std::floor(m_cameraPosition.x / 100.f) - 1;
        int endSection = (int)std::ceil((m_cameraPosition.x + winSize.x) / 100.f) + 1;

        CCRect camRect(m_cameraPosition.x, m_cameraPosition.y, winSize.x, winSize.y);
        float camLeft = camRect.getMinX();
        float camRight = camRect.getMaxX();
        float camBottom = camRect.getMinY();
        float camTop = camRect.getMaxY();

        auto transCol0 = transformColor(unk348->getColor(), 0.f, -0.3f, 0.4f);

        auto lightBgColor = m_effectManager->activeColorForIndex(1007);

        auto bgColor = m_effectManager->activeColorForIndex(1000);
        auto transformedBgColor = transformColor(bgColor, 0.f, -0.2f, 0.2f);
        bool transBgColorIsDark = transformedBgColor.r + transformedBgColor.g + transformedBgColor.b < 150u;

        float screenLeft = director->getScreenLeft();
        float screenRight = director->getScreenRight();

        float playerPos = winSize.x * 0.5f - 75.f;
        float boundLeft = playerPos - 30.f;
        float boundRight = playerPos + 110.f;
        float offsetBoundRight = screenRight - boundRight - 90.f;

        bool playerIsDead = m_player1->m_unk63F;

        CCObject* obj;
        int sectionCount = m_sectionObjects->count();

        {
            ZoneScopedN("loop 0: hide previously visible objects");
            for(auto i = m_firstVisibleSection; i <= m_lastVisibleSection; i++) {
                if(i < 0 || i >= sectionCount)
                    continue;
                auto container = reinterpret_cast<CCArray*>(m_sectionObjects->objectAtIndex(i));
                CCARRAY_FOREACH(container, obj) {
                    auto gameObject = reinterpret_cast<GameObject*>(obj);
                    gameObject->m_shouldHide = true;
                }
            }
        }

        {
            ZoneScopedN("loop 1: queue objects for update");
            for(auto i = startSection; i <= endSection; i++) {
                if(i < 0 || i >= sectionCount)
                    continue;
                auto container = reinterpret_cast<CCArray*>(m_sectionObjects->objectAtIndex(i));
                bool isOnEdge = i <= startSection + 1 || i >= endSection - 1;
                CCARRAY_FOREACH(container, obj) {
                    auto gameObject = reinterpret_cast<GameObject*>(obj);
                    if(gameObject->m_groupDisabled) {
                        gameObject->m_shouldHide = true;
                        continue;
                    }
                    if(gameObject->m_active && !isOnEdge) {
                        gameObject->m_shouldHide = false;
                        m_objectsToUpdate->addObject(obj);
                        continue;
                    }
                    auto rect = gameObject->getObjectTextureRect();
                    if(!gameObject->m_unk2e8 &&
                        (rect.getMinX() > camRight || rect.getMaxX() < camLeft ||
                        rect.getMinY() > camTop || rect.getMaxY() < camBottom)) {
                        gameObject->m_shouldHide = true;
                        continue;
                    }
                    gameObject->m_shouldHide = false;
                    m_objectsToUpdate->addObject(obj);
                }
            }
        }

        {
            ZoneScopedN("loop 2: deactivate previously visible objects");
            if(!m_fullReset) {
                for(auto i = m_firstVisibleSection; i <= m_lastVisibleSection; i++) {
                    if(i < 0 || i >= sectionCount)
                        continue;
                    auto container = reinterpret_cast<CCArray*>(m_sectionObjects->objectAtIndex(i));
                    CCARRAY_FOREACH(container, obj) {
                        auto gameObject = reinterpret_cast<GameObject*>(obj);
                        gameObject->deactivateObject(false);
                    }
                }
            }
        }

        {
            ZoneScopedN("loop 3: deactivate invisible objects");
            CCARRAY_FOREACH(m_processedGroups, obj) {
                auto gameObject = reinterpret_cast<GameObject*>(obj);
                if(gameObject->m_section < startSection || gameObject->m_section > endSection)
                    gameObject->deactivateObject(true);
            }
            m_processedGroups->removeAllObjects();
        }

        {
            ZoneScopedN("loop 4: update objects");
            float cameraCenterX = m_cameraPosition.x + *s_halfScreenWidth;
            CCARRAY_FOREACH(m_objectsToUpdate, obj) {
                auto gameObject = reinterpret_cast<GameObject*>(obj);

                updateMainColor(gameObject);
                updateSecondaryColor(gameObject);

                if(gameObject->m_active && m_hasColors.size() > 0) {
                    bool hasBaseColor = m_hasColors[gameObject->m_baseColorID];
                    bool hasDetailColor = gameObject->m_detailSprite != nullptr &&
                        m_hasColors[gameObject->m_detailColorID];
                    if(hasBaseColor || hasDetailColor) {
                        gameObject->addMainSpriteToParent(false);
                        gameObject->addColorSpriteToParent(false);
                    }
                }

                gameObject->activateObject();

                if(gameObject->m_isAnimated)
                    gameObject->updateSyncedAnimation(m_totalTime);

                CCPoint objPos = gameObject->getRealPosition();
                if(objPos.x <= cameraCenterX)
                    objPos.x += gameObject->m_rectXCenterMaybe;
                else
                    objPos.x -= gameObject->m_rectXCenterMaybe;
                float relativeMod = getRelativeModxd(this, objPos.x, 0.014285714f, 0.014285714f);

                // idk
                if(*(bool*)((uintptr_t)&gameObject->m_baseSprite + 2))
                    reinterpret_cast<AnimatedGameObject*>(gameObject)->updateChildSpriteColor(transCol0);

                // breakable block
                if(gameObject->m_objectID == 143)
                    gameObject->setGlowColor(transformedBgColor);

                // idk
                if(gameObject->m_unk364)
                    continue;

                // pulse
                if(gameObject->m_useAudioScale)
                    gameObject->setRScale(gameObject->m_customAudioScale ?
                        std::lerp(gameObject->m_minAudioScale, gameObject->m_maxAudioScale, meteringValue - 0.1f) :
                        meteringValue);

                // fade/enter
                bool ignoreFade = gameObject->m_ignoreFade;
                bool unk365 = gameObject->m_unk365;
                bool unk368 = gameObject->m_unk368;
                bool unk3b0 = gameObject->m_unk3b0;
                int objActiveEnterEffect = gameObject->m_activeEnterEffect;
                int activeEnterEffect = m_activeEnterEffect;
                gameObject->setOpacity(!ignoreFade && (((!unk365 || unk368 && unk3b0) || objActiveEnterEffect != 0 && objActiveEnterEffect != 1) || activeEnterEffect != 1) ?
                    (GLubyte)std::round(relativeMod * 255.f) : 255u);

                // idk
                if(gameObject->m_glowUserBackgroundColour)
                    gameObject->setGlowColor(gameObject->m_useSpecialLight ? bgColor : transformedBgColor);

                // invisible objects
                if(gameObject->m_invisibleMode) {
                    float invisibleRelativeMod = getRelativeModxd(this, objPos.x, 0.02f, 0.014285714f);
                    if(playerIsDead)
                        gameObject->setGlowColor(transformedBgColor);
                    else {
                        float valIdk = std::clamp(m_cameraPosition.x + boundRight - objPos.x > 0.f ?
                            (m_cameraPosition.x + playerPos - objPos.x) / std::max(boundLeft, 1.f) :
                            (objPos.x - m_cameraPosition.x - boundRight) / std::max(offsetBoundRight, 1.f), 0.f, 1.f);
                        auto mainOpacity = std::min((0.05f + valIdk * 0.95f), invisibleRelativeMod);
                        auto glowOpacity = std::min((0.15f + valIdk * 0.85f), invisibleRelativeMod);
                        gameObject->setOpacity((GLubyte)(mainOpacity * 255.f));
                        setGlowOpacity(gameObject, (GLubyte)(glowOpacity * 255.f));

                        if(mainOpacity <= 0.8f)
                            gameObject->setGlowColor(transformedBgColor);
                        else {
                            auto useCol = transBgColorIsDark ? ccColor3B{255u, 255u, 255u} : lightBgColor;
                            float t = 0.7f + (1.f - (mainOpacity - 0.8f) / 0.2f) * 0.3f;
                            gameObject->setGlowColor(getMixedColor(transformedBgColor, useCol, t));
                        }
                    }
                }

                applyEnterEffect(gameObject);

                bool isFlipping = m_mirrorTransition != 0.f && m_mirrorTransition != 1.f;
                if(!isFlipping) {
                    if(!unk460)
                        continue;
                    gameObject->setFlipX(gameObject->m_startFlipX);
                    gameObject->setFlipY(gameObject->m_startFlipY);
                    gameObject->setRotation(gameObject->m_rotation);
                    gameObject->setPosition(gameObject->getPosition());
                    continue;
                }

                // mirror animation
                float t = m_cameraFlip == -1.f ? 1.f - m_mirrorTransition : m_mirrorTransition;
                auto pos = gameObject->getPosition();
                pos.x += ((screenLeft + screenRight) - (pos.x - m_cameraPosition.x) * 2.f) * t;
                gameObject->setPosition(pos);
                float rot = std::abs(getRotation());
                bool isOnSide = rot == 90.f || rot == 270.f;
                if(m_cameraFlip == 1.f && m_mirrorTransition >= 0.5f || m_cameraFlip != 1.f && m_mirrorTransition <= 0.5f) {
                    if(isOnSide)
                        gameObject->setFlipY(!gameObject->m_startFlipY);
                    else
                        gameObject->setFlipX(!gameObject->m_startFlipX);
                    gameObject->setRotation(-gameObject->m_rotation);
                }
                else if(unk3EC && (m_cameraFlip != 1.f && m_mirrorTransition >= 0.5f ||
                    m_cameraFlip == 1.f && m_mirrorTransition <= 0.5f)) {
                    if(isOnSide)
                        gameObject->setFlipY(gameObject->m_startFlipY);
                    else
                        gameObject->setFlipX(gameObject->m_startFlipX);
                    gameObject->setRotation(m_cameraFlip == 1.f ? gameObject->m_rotation : -gameObject->m_rotation);
                }
            }
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
