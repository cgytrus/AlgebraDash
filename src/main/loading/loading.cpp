#include "../../includes.h"
#include "../../ThreadPool.hpp"
#include <mutex>

const CCTexture2DPixelFormat kCCTexture2DPixelFormat_DontChange = (CCTexture2DPixelFormat)-1;
std::mutex loadedImagesMutex;
typedef void(*LoadedImageCallback)(CCTexture2D* texture);
struct LoadedImage {
    CCImage* image;
    const char* plistPath;
    std::string pathKey;
    CCTexture2DPixelFormat pixelFormat;
    LoadedImageCallback callback;
};
std::vector<LoadedImage> loadedImages;
void addImage(CCTextureCache* self, const char* path, const char* plistPath, LoadedImageCallback callback = nullptr,
    CCTexture2DPixelFormat pixelFormat = kCCTexture2DPixelFormat_DontChange) {
    ZoneScoped

    CCImage* pImage = nullptr;

    std::string pathKey = path;
    pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str(), false);
    if(pathKey.size() == 0)
        return;

    auto m_pTextures = (CCDictionary**)((uintptr_t)self + 0x20);
    CCTexture2D* texture = (CCTexture2D*)(*m_pTextures)->objectForKey(pathKey.c_str());
    if(texture) {
        loadedImages.push_back({pImage, plistPath, pathKey, texture->getPixelFormat(), callback});
        return;
    }

    std::string fullpath = pathKey;
    std::string lowerCase(pathKey);
    for(unsigned int i = 0; i < lowerCase.length(); ++i)
        lowerCase[i] = tolower(lowerCase[i]);

    if(std::string::npos != lowerCase.find(".pvr")) {
        self->addPVRImage(fullpath.c_str());
        return;
    }

    if(std::string::npos != lowerCase.find(".pkm")) {
        self->addETCImage(fullpath.c_str());
        return;
    }

    CCImage::EImageFormat eImageFormat = CCImage::kFmtUnKnown;
    if(std::string::npos != lowerCase.find(".png"))
        eImageFormat = CCImage::kFmtPng;
    else if(std::string::npos != lowerCase.find(".jpg") || std::string::npos != lowerCase.find(".jpeg"))
        eImageFormat = CCImage::kFmtJpg;
    else if(std::string::npos != lowerCase.find(".tif") || std::string::npos != lowerCase.find(".tiff"))
        eImageFormat = CCImage::kFmtTiff;
    else if(std::string::npos != lowerCase.find(".webp"))
        eImageFormat = CCImage::kFmtWebp;

    ThreadPool::sharedPool()->queueJob([self, fullpath, eImageFormat, pathKey, plistPath, callback, pixelFormat] {
        ZoneScopedN("init image job")

        auto pImage = new CCImage();
        if(!pImage)
            return;

        bool bRet = pImage->initWithImageFileThreadSafe(fullpath.c_str(), eImageFormat);
        if(!bRet) {
            pImage->release();
            return;
        }

        {
            std::unique_lock<std::mutex> lock(loadedImagesMutex);
            loadedImages.push_back({pImage, plistPath, pathKey, pixelFormat, callback});
        }
    });
}

void addFont(CCTextureCache* textureCache, const char* fontFile) {
    ZoneScoped
    auto fontConf = FNTConfigLoadFile(fontFile);
    if(!fontConf)
        return;
    fontConf->retain();
    addImage(textureCache, fontConf->getAtlasName(), nullptr);
    ThreadPool::sharedPool()->finishQueue();
    //addImage(textureCache, /*fontConf->getAtlasName()*/atlasFile, nullptr, [](CCTexture2D* texture) {
        //CCLabelBMFont::create(" ", fontFile);
        //CCSpriteBatchNode::createWithTexture(texture, 1);
    //});
}

class CCContentManager : public CCObject {
public:
    static CCContentManager* sharedManager();

    CCDictionary* addDict(const char* name, bool);
};

int maxProgress = 0;
void (__thiscall* LoadingLayer_loadAssets)(gd::LoadingLayer*);
void __fastcall LoadingLayer_loadAssets_H(gd::LoadingLayer* self) {
    auto m_nLoadIndex = (int*)((uintptr_t)self + 0x120);
    auto m_bFromRefresh = (bool*)((uintptr_t)self + 0x138);
    auto application = CCApplication::sharedApplication();
    auto director = CCDirector::sharedDirector();
    auto textureCache = CCTextureCache::sharedTextureCache();
    auto spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();
    auto contentManager = CCContentManager::sharedManager();
    auto gameManager = gd::GameManager::sharedState();
    if(*m_nLoadIndex < 1) {
        auto m_bUseRobtopDumbFormatForColorPicker = (bool*)((uintptr_t)application + 0xa7);

        // 1
        {
            ZoneScopedN("load assets 1")
            addImage(textureCache, "GJ_GameSheet.png", "GJ_GameSheet.plist");
        }

        // 2
        {
            ZoneScopedN("load assets 2")
            addImage(textureCache, "GJ_GameSheet02.png", "GJ_GameSheet02.plist");
        }

        // 3
        {
            ZoneScopedN("load assets 3")
            addImage(textureCache, "GJ_GameSheet03.png", "GJ_GameSheet03.plist");
        }

        // 4
        {
            ZoneScopedN("load assets 4")
            addImage(textureCache, "GJ_GameSheet04.png", "GJ_GameSheet04.plist");
        }

        // 5
        {
            ZoneScopedN("load assets 5")
            addImage(textureCache, "GJ_GameSheetGlow.png", "GJ_GameSheetGlow.plist");
            addImage(textureCache, "FireSheet_01.png", "FireSheet_01.plist");
            addImage(textureCache, "GJ_ShopSheet.png", "GJ_ShopSheet.plist");
            addImage(textureCache, "smallDot.png", nullptr);
            addImage(textureCache, "square02_001.png", nullptr);
        }

        // 6
        {
            ZoneScopedN("load assets 6")
            addImage(textureCache, "CCControlColourPickerSpriteSheet.png", "CCControlColourPickerSpriteSheet.plist", nullptr,
                *m_bUseRobtopDumbFormatForColorPicker ? kCCTexture2DPixelFormat_DontChange : CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            addImage(textureCache, "GJ_gradientBG.png", nullptr, nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            addImage(textureCache, "edit_barBG_001.png", nullptr, nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            addImage(textureCache, "GJ_button_01.png", nullptr, nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            addImage(textureCache, "slidergroove2.png", nullptr, nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            addImage(textureCache, "sliderBar2.png", nullptr, nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        }

        // 7
        {
            ZoneScopedN("load assets 7")
            addImage(textureCache, "GJ_square01.png", nullptr);
            addImage(textureCache, "GJ_square02.png", nullptr);
            addImage(textureCache, "GJ_square03.png", nullptr);
            addImage(textureCache, "GJ_square04.png", nullptr);
            addImage(textureCache, "GJ_square05.png", nullptr);
            addImage(textureCache, "gravityLine_001.png", nullptr);
        }

        maxProgress = ThreadPool::sharedPool()->getJobCount();
        ThreadPool::sharedPool()->finishQueue();

        // 8
        {
            ZoneScopedN("load assets 8")
            addFont(textureCache, "chatFont.fnt");
            //CCLabelBMFont::create(" ", "chatFont.fnt");
            CCLabelBMFont::create(" ", "goldFont.fnt");
            CCLabelBMFont::create(" ", "bigFont.fnt");
            //addFont(textureCache, "chatFont.png", "chatFont.fnt");
            //addFont(textureCache, "goldFont.png", "goldFont.fnt");
            //addFont(textureCache, "bigFont.png", "bigFont.fnt");
            //addImage(textureCache, "chatFont.png", nullptr, [](CCTexture2D* texture) { CCLabelBMFont::create(" ", "chatFont.fnt"); });
            //addImage(textureCache, "goldFont.png", nullptr, [](CCTexture2D* texture) { CCLabelBMFont::create(" ", "goldFont.fnt"); });
            //addImage(textureCache, "bigFont.png", nullptr, [](CCTexture2D* texture) { CCLabelBMFont::create(" ", "bigFont.fnt"); });
        }

        // 9
        {
            ZoneScopedN("load assets 9")
            class ObjectManager;
            class CCAnimateFrameCache;

            // ObjectManager::sharedObjectManager
            auto objectManager = reinterpret_cast<ObjectManager* (__cdecl*)()>(gd::base + 0x2c2c0)();
            // idk what's this one probably some init or smth too lazy to figure out
            reinterpret_cast<void (__thiscall*)(ObjectManager*)>(gd::base + 0x2c3b0)(objectManager);

            // CCAnimateFrameCache::sharedAnimateFrameCache
            auto animateFrameCache = reinterpret_cast<CCAnimateFrameCache* (__cdecl*)()>(gd::base + 0x158f0)();
            // CCAnimateFrameCache::addAnimation?
            reinterpret_cast<void (__thiscall*)(CCAnimateFrameCache*, const char*)>(gd::base + 0x159b0)(animateFrameCache, "Robot_AnimDesc.plist");

            contentManager->addDict("glassDestroy01.plist", false);
            contentManager->addDict("coinPickupEffect.plist", false);
            contentManager->addDict("explodeEffect.plist", false);

            gd::AchievementManager::sharedState();
            gd::ObjectToolbox::sharedState();
        }

        // 10
        // it does nothing lol
        {
            ZoneScopedN("load assets 10")
        }

        // 11
        // what.
        {
            ZoneScopedN("load assets 11")
            if(gameManager->m_bClickedEditor && !*m_bFromRefresh)
                gameManager->m_bShowProgressBar = true;
        }

        // 12
        {
            ZoneScopedN("load assets 12")
            gd::CCTextInputNode::create("Temp", self, "bigFont.fnt", 200.f, 50.f);
        }

        // 13
        {
            ZoneScopedN("load assets 13")
            if(!*m_bFromRefresh) {
                auto circleWave = gd::CCCircleWave::create(true, true, 10.f, 5.f, 0.3f);
                self->addChild(circleWave);
                auto idk = CCPoint(-100.f, -100.f);
                circleWave->idk(&idk);
            }
        }

        *m_nLoadIndex = 14;
    }

    {
        ZoneScopedN("finish add image")
        std::unique_lock<std::mutex> lock(loadedImagesMutex);

        auto savedPixelFormat = CCTexture2D::defaultAlphaPixelFormat();
        for(auto loadedImage : loadedImages) {
            auto texture = new CCTexture2D();
            if(!texture)
                continue;
            CCTexture2D::setDefaultAlphaPixelFormat(loadedImage.pixelFormat < 0 ? savedPixelFormat : loadedImage.pixelFormat);
            if(texture->initWithImage(loadedImage.image)) {
                auto m_pTextures = (CCDictionary**)((uintptr_t)textureCache + 0x20);
                (*m_pTextures)->setObject(texture, loadedImage.pathKey.c_str());
                if(loadedImage.plistPath)
                    spriteFrameCache->addSpriteFramesWithFile(loadedImage.plistPath, texture);
            }
            if(loadedImage.callback)
                loadedImage.callback(texture);
            texture->release();
        }
        CCTexture2D::setDefaultAlphaPixelFormat(savedPixelFormat);

        loadedImages.clear();
    }

    if(ThreadPool::sharedPool()->getJobCount() <= 0) {
        director->setDelegate((CCDirectorDelegate*)((uintptr_t)application + 0x98)); // idk what 98 is
        *(bool*)((uintptr_t)application + 0xa4) = true; // idk what a4 is either
        if(!*m_bFromRefresh)
            gameManager->fadeInMusic("menuLoop.mp3");
        self->openMenuLayer();
    }
    else {
        ZoneScopedN("wait")

        auto m_fSliderGrooveXPos = (float*)((uintptr_t)self + 0x130);
        float sliderWidth = *m_fSliderGrooveXPos;
        *(bool*)((uintptr_t)self + 0x11d) = true;
        float progress = (1.f - (float)ThreadPool::sharedPool()->getJobCount() / maxProgress) * sliderWidth;
        if(progress <= sliderWidth)
            sliderWidth = progress;
        auto m_pSliderBar = (CCSprite**)((uintptr_t)self + 0x12c);
        auto m_fSliderHeight = (float*)((uintptr_t)self + 0x134);
        auto newRect = CCRect(0.f, 0.f, sliderWidth, *m_fSliderHeight);
        (*m_pSliderBar)->setTextureRect(newRect);

        union {
            uintptr_t loadAssetsPtr;
            SEL_CallFunc loadAssetsSelector;
        };
        loadAssetsPtr = gd::base + 0x18c8e0;
        CCCallFunc* callFunc = CCCallFunc::create(self, loadAssetsSelector);
        CCDelayTime* delayTime = CCDelayTime::create(0.01f);
        CCSequence* sequence = CCSequence::create(delayTime, callFunc, nullptr);
        gd::GameManager::sharedState()->getActionManager()->addAction(sequence, self, false);
    }
}

#include "loading.h"
void initLoadingOptimizations(uintptr_t base) {
    MH_CreateHook(reinterpret_cast<void*>(base + 0x18c8e0),
        reinterpret_cast<void*>(&LoadingLayer_loadAssets_H),
        reinterpret_cast<void**>(&LoadingLayer_loadAssets));
}
