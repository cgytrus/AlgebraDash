#include "../../includes.h"
#include <mutex>

const CCTexture2DPixelFormat kCCTexture2DPixelFormat_DontChange = (CCTexture2DPixelFormat)-1;

struct LoadedImage {
    CCImage* image;
    const char* plistPath;
    std::string pathKey;
    CCTexture2DPixelFormat pixelFormat;
};
std::mutex loadedImagesMutex;
std::vector<LoadedImage> loadedImages;
void addImage(CCTextureCache* self, const char* path, const char* plistPath, CCTexture2DPixelFormat pixelFormat = kCCTexture2DPixelFormat_DontChange) {
    ZoneScoped;

    CCImage* pImage = nullptr;

    std::string pathKey = path;
    pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str(), false);
    if(pathKey.size() == 0)
        return;

    auto textures = *(CCDictionary**)((uintptr_t)self + 0x20);
    CCTexture2D* texture = (CCTexture2D*)textures->objectForKey(pathKey.c_str());
    if(texture) {
        {
            std::unique_lock<std::mutex> lock(loadedImagesMutex);
            loadedImages.push_back({pImage, plistPath, pathKey, texture->getPixelFormat()});
        }
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

    sharedPool()->push_task([self, fullpath, eImageFormat, pathKey, plistPath, pixelFormat] {
        ZoneScopedN("init image job");

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
            loadedImages.push_back({pImage, plistPath, pathKey, pixelFormat});
        }
    });
}

void addFont(CCTextureCache* textureCache, const char* fontFile) {
    ZoneScoped;
    auto fontConf = FNTConfigLoadFile(fontFile);
    if(!fontConf)
        return;
    fontConf->retain();
    addImage(textureCache, fontConf->getAtlasName(), nullptr);
}

class CCContentManager : public CCObject {
public:
    static CCContentManager* sharedManager();
    CCDictionary* addDict(const char* name, bool);
};

int maxProgress = 0;
void load(gd::LoadingLayer* self) {
    ZoneScoped;
    auto textureCache = CCTextureCache::sharedTextureCache();

    // 1
    {
        ZoneScopedN("load assets 1");
        addImage(textureCache, "GJ_GameSheet.png", "GJ_GameSheet.plist");
    }

    // 2
    {
        ZoneScopedN("load assets 2");
        addImage(textureCache, "GJ_GameSheet02.png", "GJ_GameSheet02.plist");
    }

    // 3
    {
        ZoneScopedN("load assets 3");
        addImage(textureCache, "GJ_GameSheet03.png", "GJ_GameSheet03.plist");
    }

    // 4
    {
        ZoneScopedN("load assets 4");
        addImage(textureCache, "GJ_GameSheet04.png", "GJ_GameSheet04.plist");
    }

    // 5
    {
        ZoneScopedN("load assets 5");
        addImage(textureCache, "GJ_GameSheetGlow.png", "GJ_GameSheetGlow.plist");
        addImage(textureCache, "FireSheet_01.png", "FireSheet_01.plist");
        addImage(textureCache, "GJ_ShopSheet.png", "GJ_ShopSheet.plist");
        addImage(textureCache, "smallDot.png", nullptr);
        addImage(textureCache, "square02_001.png", nullptr);
    }

    // 6
    {
        ZoneScopedN("load assets 6");
        auto useRobtopDumbFormatForColorPicker = *(bool*)((uintptr_t)CCApplication::sharedApplication() + 0xa7);
        addImage(textureCache, "CCControlColourPickerSpriteSheet.png", "CCControlColourPickerSpriteSheet.plist",
            useRobtopDumbFormatForColorPicker ? kCCTexture2DPixelFormat_DontChange : CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        addImage(textureCache, "GJ_gradientBG.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        addImage(textureCache, "edit_barBG_001.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        addImage(textureCache, "GJ_button_01.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        addImage(textureCache, "slidergroove2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        addImage(textureCache, "sliderBar2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
    }

    // 7
    {
        ZoneScopedN("load assets 7");
        addImage(textureCache, "GJ_square01.png", nullptr);
        addImage(textureCache, "GJ_square02.png", nullptr);
        addImage(textureCache, "GJ_square03.png", nullptr);
        addImage(textureCache, "GJ_square04.png", nullptr);
        addImage(textureCache, "GJ_square05.png", nullptr);
        addImage(textureCache, "gravityLine_001.png", nullptr);
    }

    // 8
    {
        ZoneScopedN("load assets 8");
        addFont(textureCache, "chatFont.fnt");
        addFont(textureCache, "goldFont.fnt");
        addFont(textureCache, "bigFont.fnt");
    }

    maxProgress = sharedPool()->get_tasks_total();

    // 9
    {
        ZoneScopedN("load assets 9");
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

        auto contentManager = CCContentManager::sharedManager();
        contentManager->addDict("glassDestroy01.plist", false);
        contentManager->addDict("coinPickupEffect.plist", false);
        contentManager->addDict("explodeEffect.plist", false);

        gd::AchievementManager::sharedState();
        gd::ObjectToolbox::sharedState();
    }

    // 10
    // it does nothing lol

    // 11
    // what.
    {
        ZoneScopedN("load assets 11");
        auto gameManager = gd::GameManager::sharedState();
        if(gameManager->m_bClickedEditor && !self->m_bFromRefresh)
            gameManager->m_bShowProgressBar = true;
    }

    // 12
    {
        ZoneScopedN("load assets 12");
        gd::CCTextInputNode::create("Temp", self, "bigFont.fnt", 200.f, 50.f);
    }

    // 13
    {
        ZoneScopedN("load assets 13");
        if(!self->m_bFromRefresh) {
            auto circleWave = gd::CCCircleWave::create(true, true, 10.f, 5.f, 0.3f);
            self->addChild(circleWave);
            auto idk = CCPoint(-100.f, -100.f);
            circleWave->idk(&idk);
        }
    }
}

void finishLoading(gd::LoadingLayer* self) {
    ZoneScopedN("load assets 14");

    auto application = CCApplication::sharedApplication();
    CCDirector::sharedDirector()->setDelegate((CCDirectorDelegate*)((uintptr_t)application + 0x98)); // idk what 98 is
    *(bool*)((uintptr_t)application + 0xa4) = true; // idk what a4 is either

    if(!self->m_bFromRefresh)
        gd::GameManager::sharedState()->fadeInMusic("menuLoop.mp3");

    self->loadingFinished();
}

void finishAddImage() {
    ZoneScoped;
    std::unique_lock<std::mutex> lock(loadedImagesMutex);

    auto textureCache = CCTextureCache::sharedTextureCache();
    auto spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();

    auto savedPixelFormat = CCTexture2D::defaultAlphaPixelFormat();
    for(auto loadedImage : loadedImages) {
        auto texture = new CCTexture2D();
        if(!texture) {
            CC_SAFE_RELEASE(loadedImage.image);
            continue;
        }

        CCTexture2D::setDefaultAlphaPixelFormat(loadedImage.pixelFormat < 0 ? savedPixelFormat : loadedImage.pixelFormat);
        if(texture->initWithImage(loadedImage.image)) {
            auto textures = *(CCDictionary**)((uintptr_t)textureCache + 0x20);
            textures->setObject(texture, loadedImage.pathKey.c_str());
            if(loadedImage.plistPath)
                spriteFrameCache->addSpriteFramesWithFile(loadedImage.plistPath, texture);
        }

        texture->release();
        CC_SAFE_RELEASE(loadedImage.image);
    }
    CCTexture2D::setDefaultAlphaPixelFormat(savedPixelFormat);

    loadedImages.clear();
}

void updateProgressBar(gd::LoadingLayer* self) {
    ZoneScoped;
    auto sliderX = self->m_fSliderGrooveXPos;

    self->unk11D = true;

    float progress = (1.f - (float)sharedPool()->get_tasks_total() / maxProgress) * sliderX;
    if(progress <= sliderX)
        sliderX = progress;

    self->m_pSliderBar->setTextureRect({ 0.f, 0.f, sliderX, self->m_fSliderGrooveHeight });
}

void runLoadAssets(gd::LoadingLayer* self) {
    ZoneScoped;
    union {
        uintptr_t ptr;
        SEL_CallFunc selector;
    } loadAssets = { gd::base + 0x18c8e0 };

    CCCallFunc* callFunc = CCCallFunc::create(self, loadAssets.selector);
    CCDelayTime* delayTime = CCDelayTime::create(0.f);
    CCSequence* sequence = CCSequence::create(delayTime, callFunc, nullptr);
    gd::GameManager::sharedState()->getActionManager()->addAction(sequence, self, false);
}

matdash::cc::thiscall<void> LoadingLayer_loadAssets(gd::LoadingLayer* self) {
    ZoneScoped;
    if(self->m_nLoadStep <= 0) {
        load(self);
        self->m_nLoadStep = 14;
    }

    finishAddImage();

    if(sharedPool()->get_tasks_total() > 0) {
        ZoneScopedN("wait");
        updateProgressBar(self);
        runLoadAssets(self);
    }
    else
        finishLoading(self);

    return {};
}

#include "loading.h"
void initLoadingOptimizations() {
    matdash::add_hook<&LoadingLayer_loadAssets>(gd::base + 0x18c8e0);
}
