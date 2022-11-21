#include "../../includes.h"

matdash::cc::thiscall<void> CCSpriteBatchNode_draw(CCSpriteBatchNode* self) {
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

    return {};
}

void (__thiscall* updateAtlasIndex)(CCSpriteBatchNode*, CCSprite*, int*);
matdash::cc::thiscall<void> CCSpriteBatchNode_sortAllChildren(CCSpriteBatchNode* self) {
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

    return {};
}

static inline void transformColor(ccColor3B& color, float h, float s, float v) {
    gd::GameToolbox::transformColor(color, color, cchsv(h, s, v, true, true));
}

float opacityModForMode(gd::GameObject* self, int mode, bool isMain) {
    ZoneScoped;

    float res = 1.f;
    if(mode > 0) {
        auto colorActionSprite = isMain ? self->m_pColorActionSprite1 : self->m_pColorActionSprite2;
        auto opacity = colorActionSprite->m_opacity;
        if(opacity < 250.f)
            res = opacity * 0.004f;
    }
    if(self->m_alphaGroupCount > 0)
        res *= self->groupOpacityMod();
    return res;
}

void updateMainColor(gd::GameObject* self) {
    ZoneScoped;

    auto colorMode = self->m_baseColorID;
    if(colorMode == 0 && self->m_pulseGroupCount == 0 && self->m_alphaGroupCount == 0)
        return;
    auto color = self->colorForMode((int)colorMode, true);
    self->updateMainColor(color);
    self->m_pBaseColor->unk_0F4 = opacityModForMode(self, (int)colorMode, true);
}

void updateSecondaryColor(gd::GameObject* self) {
    ZoneScoped;

    bool hasSecondaryColor = self->m_pDetailSprite != nullptr;
    auto colorMode = self->m_detailColorID;
    if(!hasSecondaryColor || colorMode == 0 && self->m_nGroupCount == 0)
        return;
    auto color = self->colorForMode((int)colorMode, false);
    self->updateSecondaryColor(color);
    self->m_pDetailColor->unk_0F4 = opacityModForMode(self, (int)colorMode, false);
}

float* g_halfScreenWidth;

float getRelativeMod(gd::PlayLayer* self, float objX, float outSpeed, float inSpeed) {
    ZoneScoped;

    float cameraCenterX = self->m_cameraX + *g_halfScreenWidth;
    return std::clamp(objX <= cameraCenterX ? (objX - self->m_cameraX) * inSpeed :
        (cameraCenterX + *g_halfScreenWidth - objX) * outSpeed, 0.f, 1.f);
}

void updateGameObject(gd::PlayLayer* self, gd::GameObject* gameObject, float meteringValue,
    ccColor3B transCol0, ccColor3B bgColor, ccColor3B transformedBgColor, ccColor3B lightBgColor,
    float screenLeft, float screenRight, float playerPos, float boundLeft, float boundRight, float offsetBoundRight) {
    updateMainColor(gameObject);
    updateSecondaryColor(gameObject);

    if(gameObject->m_bActive && self->m_hasColors.size() > 0) {
        bool hasBaseColor = self->m_hasColors[gameObject->m_baseColorID];
        bool hasDetailColor = gameObject->m_pDetailSprite != nullptr &&
            self->m_hasColors[gameObject->m_detailColorID];
        if(hasBaseColor || hasDetailColor) {
            gameObject->addMainSpriteToParent(false);
            gameObject->addColorSpriteToParent(false);
        }
    }

    gameObject->activateObject();

    if(gameObject->m_isAnimated)
        gameObject->updateSyncedAnimation(self->m_totalTime);

    CCPoint objPos;
    gameObject->getRealPosition(objPos);
    if(objPos.x <= (self->m_cameraX + *g_halfScreenWidth))
        objPos.x += gameObject->m_fRectXCenterMaybe;
    else
        objPos.x -= gameObject->m_fRectXCenterMaybe;
    float relativeMod = getRelativeMod(self, objPos.x, 0.014285714f, 0.014285714f);

    // idk
    if(gameObject->m_unk2e2)
        gameObject->updateChildSpriteColor(transCol0);

    // breakable block
    if(gameObject->m_nObjectID == 143)
        gameObject->setGlowColor(transformedBgColor);

    // idk
    if(gameObject->m_unk364)
        return;

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
    int activeEnterEffect = gameObject->m_activeEnterEffect;
    int unk2ac = self->unk2AC;
    gameObject->setOpacity(!ignoreFade && (((!unk365 || unk368 && unk3b0) || activeEnterEffect != 0 && activeEnterEffect != 1) || unk2ac != 1) ?
        (GLubyte)std::round(relativeMod * 255.f) : 255u);

    // idk
    if(gameObject->m_glowUserBackgroundColour)
        gameObject->setGlowColor(gameObject->m_useSpecialLight ? bgColor : transformedBgColor);

    // invisible objects
    if(gameObject->m_invisibleMode) {
        float invisibleRelativeMod = getRelativeMod(self, objPos.x, 0.02f, 0.014285714f);
        if(self->m_pPlayer1->m_isDead)
            gameObject->setGlowColor(transformedBgColor);
        else {
            float valIdk = std::clamp(self->m_cameraX + boundRight - objPos.x > 0.f ?
                (self->m_cameraX + playerPos - objPos.x) / std::max(boundLeft, 1.f) :
                (objPos.x - self->m_cameraX - boundRight) / std::max(offsetBoundRight, 1.f), 0.f, 1.f);
            auto mainOpacity = std::min((0.05f + valIdk * 0.95f), invisibleRelativeMod);
            auto glowOpacity = std::min((0.15f + valIdk * 0.85f), invisibleRelativeMod);
            gameObject->setOpacity((GLubyte)(mainOpacity * 255.f));
            gameObject->setGlowOpacity((GLubyte)(glowOpacity * 255.f));

            if(mainOpacity <= 0.8f)
                gameObject->setGlowColor(transformedBgColor);
            else {
                bool transBgColorIsDark = transformedBgColor.r + transformedBgColor.g + transformedBgColor.b < 150u;
                auto useCol = transBgColorIsDark ? ccColor3B{255u, 255u, 255u} : lightBgColor;
                float t = 0.7f + (1.f - (mainOpacity - 0.8f) / 0.2f) * 0.3f;
                gd::GJEffectManager::getMixedColor(useCol, transformedBgColor, useCol, t);
                gameObject->setGlowColor(useCol);
            }
        }
    }

    self->applyEnterEffect(gameObject);

    bool isFlipping = self->m_mirrorTransition != 0.f && self->m_mirrorTransition != 1.f;
    if(!isFlipping) {
        if(!self->unk460)
            return;
        gameObject->setFlipX(gameObject->m_startFlipX);
        gameObject->setFlipY(gameObject->m_startFlipY);
        gameObject->setRotation(gameObject->m_rotation);
        gameObject->setPosition(gameObject->getPosition());
        return;
    }

    // mirror animation
    float t = self->m_cameraFlip == -1.f ? 1.f - self->m_mirrorTransition : self->m_mirrorTransition;
    auto pos = gameObject->getPosition();
    pos.x += ((screenLeft + screenRight) - (pos.x - self->m_cameraX) * 2.f) * t;
    gameObject->setPosition(pos);
    float rot = std::abs(self->getRotation());
    bool isOnSide = rot == 90.f || rot == 270.f;
    if(self->m_cameraFlip == 1.f && self->m_mirrorTransition >= 0.5f ||
       self->m_cameraFlip != 1.f && self->m_mirrorTransition <= 0.5f) {
        if(isOnSide)
            gameObject->setFlipY(!gameObject->m_startFlipY);
        else
            gameObject->setFlipX(!gameObject->m_startFlipX);
        gameObject->setRotation(-gameObject->m_rotation);
    }
    else if(self->unk3EC && (self->m_cameraFlip != 1.f && self->m_mirrorTransition >= 0.5f ||
        self->m_cameraFlip == 1.f && self->m_mirrorTransition <= 0.5f)) {
        if(isOnSide)
            gameObject->setFlipY(gameObject->m_startFlipY);
        else
            gameObject->setFlipX(gameObject->m_startFlipX);
        gameObject->setRotation(gameObject->m_rotation * self->m_cameraFlip);
    }
}

matdash::cc::thiscall<void> PlayLayer_updateVisibility(gd::PlayLayer* self) {
    ZoneScoped;

#ifdef CUSTOM_DEBUG
    // use vanilla if show percentage is off
    if(!gd::GameManager::sharedState()->getGameVariable("0040")) {
        matdash::orig<&PlayLayer_updateVisibility>(self);
        return {};
    }
#endif

    *getAt<float>(self->m_pEffectManager, 0x1b6c) = self->m_totalTime; // 0x1b6c = m_time

    self->m_pEffectManager->processColors();

    auto player1Color = self->m_pEffectManager->activeColorForIndex(1005);
    self->m_pEffectManager->calculateLightBGColor(player1Color);

    float meteringValue = self->m_isPracticeMode || self->m_isMute ? 0.5f :
        self->unk461 ? gd::FMODAudioEngine::sharedEngine()->m_fPulse1 :
            *getAt<float>(self->m_audioEffectsLayer, 0x1b0);

    self->m_pPlayer1->unk694 = meteringValue;
    self->m_pPlayer2->unk694 = meteringValue;

    auto director = CCDirector::sharedDirector();
    CCPoint winSize = director->getWinSize();
    int startSection = (int)std::floor(self->m_cameraX / 100.f) - 1;
    int endSection = (int)std::ceil((self->m_cameraX + winSize.x) / 100.f) + 1;

    CCRect camRect(self->m_cameraX, self->m_cameraY, winSize.x, winSize.y);
    float camLeft = camRect.getMinX();
    float camRight = camRect.getMaxX();
    float camBottom = camRect.getMinY();
    float camTop = camRect.getMaxY();

    auto transCol0 = self->unk348->getColor();
    transformColor(transCol0, 0.f, -0.3f, 0.4f);

    auto lightBgColor = self->m_pEffectManager->activeColorForIndex(1007);

    auto bgColor = self->m_pEffectManager->activeColorForIndex(1000);
    auto transformedBgColor = bgColor;
    transformColor(transformedBgColor, 0.f, -0.2f, 0.2f);

    float screenLeft = director->getScreenLeft();
    float screenRight = director->getScreenRight();

    float playerPos = winSize.x * 0.5f - 75.f;
    float boundLeft = playerPos - 30.f;
    float boundRight = playerPos + 110.f;
    float offsetBoundRight = screenRight - boundRight - 90.f;

    CCObject* obj;
    int sectionCount = self->m_pObjectContainerArrays->count();

    {
        ZoneScopedN("loop 0: hide previously visible objects");
        for(auto i = self->m_firstVisibleSection; i <= self->m_lastVisibleSection; i++) {
            if(i < 0 || i >= sectionCount)
                continue;
            auto container = reinterpret_cast<CCArray*>(self->m_pObjectContainerArrays->objectAtIndex(i));
            CCARRAY_FOREACH(container, obj) {
                auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
                gameObject->m_shouldHide = true;
            }
        }
    }

    {
        ZoneScopedN("loop 1: queue objects for update");
        for(auto i = startSection; i <= endSection; i++) {
            if(i < 0 || i >= sectionCount)
                continue;
            auto container = reinterpret_cast<CCArray*>(self->m_pObjectContainerArrays->objectAtIndex(i));
            bool isOnEdge = i <= startSection + 1 || i >= endSection - 1;
            CCARRAY_FOREACH(container, obj) {
                auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
                if(gameObject->m_bGroupDisabled) {
                    gameObject->m_shouldHide = true;
                    continue;
                }
                if(gameObject->m_bActive && !isOnEdge) {
                    gameObject->m_shouldHide = false;
                    self->m_objectsToUpdate->addObject(obj);
                    continue;
                }
                auto rect = gameObject->getObjectTextureRect();
                if(!gameObject->m_unk2e8 &&
                    (rect->getMinX() > camRight || rect->getMaxX() < camLeft ||
                    rect->getMinY() > camTop || rect->getMaxY() < camBottom)) {
                    gameObject->m_shouldHide = true;
                    continue;
                }
                gameObject->m_shouldHide = false;
                self->m_objectsToUpdate->addObject(obj);
            }
        }
    }

    {
        ZoneScopedN("loop 2: deactivate previously visible objects");
        if(!self->unk310) {
            /*auto threadPool = sharedPool();
            int start = std::max(self->m_firstVisibleSection, 0);
            int end = std::min(self->m_lastVisibleSection + 1, sectionCount);
            BS::multi_future<void> job =
                threadPool->parallelize_loop(start, end, [self, threadPool](int a, int b) {
                    ZoneScopedN("job");
                    for(int i = a; i < b; ++i) {
                        auto container = reinterpret_cast<CCArray*>(self->m_pObjectContainerArrays->objectAtIndex(i));
                        CCObject* obj;
                        CCARRAY_FOREACH(container, obj) {
                            auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
                            gameObject->deactivateObject(false);
                        }
                    }
                });
            {
                ZoneScopedN("wait");
                job.wait();
            }*/
            for(auto i = self->m_firstVisibleSection; i <= self->m_lastVisibleSection; i++) {
                if(i < 0 || i >= sectionCount)
                    continue;
                auto container = reinterpret_cast<CCArray*>(self->m_pObjectContainerArrays->objectAtIndex(i));
                CCARRAY_FOREACH(container, obj) {
                    auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
                    gameObject->deactivateObject(false);
                }
            }
        }
    }

    {
        ZoneScopedN("loop 3: deactivate invisible objects");
        CCARRAY_FOREACH(self->m_processedGroups, obj) {
            auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
            if(gameObject->m_nSection < startSection || gameObject->m_nSection > endSection)
                gameObject->deactivateObject(true);
        }
        self->m_processedGroups->removeAllObjects();
    }

    {
        ZoneScopedN("loop 4: update objects");
        /*auto threadPool = sharedPool();
        CCArray* array = self->m_objectsToUpdate;
        CCObject** objects = array->data->arr;
        BS::multi_future<void> job =
            threadPool->parallelize_loop(0, array->data->num, [objects, self, meteringValue,
                    transCol0, bgColor, transformedBgColor, lightBgColor, screenLeft, screenRight,
                    playerPos, boundLeft, boundRight, offsetBoundRight](unsigned int a, unsigned int b) {
                ZoneScopedN("update objects job");
                for(unsigned int i = a; i < b; ++i) {
                    auto gameObject = reinterpret_cast<gd::GameObject*>(objects[i]);
                    updateGameObject(self, gameObject, meteringValue,
                        transCol0, bgColor, transformedBgColor, lightBgColor,
                        screenLeft, screenRight, playerPos, boundLeft, boundRight, offsetBoundRight);
                }
            });
        {
            ZoneScopedN("wait");
            job.wait();
        }*/
        CCARRAY_FOREACH(self->m_objectsToUpdate, obj) {
            auto gameObject = reinterpret_cast<gd::GameObject*>(obj);
            updateGameObject(self, gameObject, meteringValue,
                transCol0, bgColor, transformedBgColor, lightBgColor,
                screenLeft, screenRight, playerPos, boundLeft, boundRight, offsetBoundRight);
        }
        self->m_objectsToUpdate->removeAllObjects();
    }

    self->m_firstVisibleSection = startSection;
    self->m_lastVisibleSection = endSection;
    self->unk310 = false;

    self->unk460 = false;
    self->unk464->removeAllObjects();
    self->m_hasColors.clear();

    return {};
}

#include "rendering.h"
void initRenderingOptimizations() {
    matdash::add_hook<&CCSpriteBatchNode_draw>(CC_ADDR("?draw@CCSpriteBatchNode@cocos2d@@UAEXXZ"));
    matdash::add_hook<&CCSpriteBatchNode_sortAllChildren>(CC_ADDR("?sortAllChildren@CCSpriteBatchNode@cocos2d@@UAEXXZ"));

    updateAtlasIndex = reinterpret_cast<void(__thiscall*)(CCSpriteBatchNode*, CCSprite*, int*)>
        (CC_ADDR("?updateAtlasIndex@CCSpriteBatchNode@cocos2d@@AAEXPAVCCSprite@2@PAH@Z"));

    //g_halfScreenWidth = reinterpret_cast<float*>(gd::base + 0x3222f4);
    //matdash::add_hook<&PlayLayer_updateVisibility>(gd::base + 0x205460);
}
