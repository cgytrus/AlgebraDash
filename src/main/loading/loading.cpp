#include <Tracy.hpp>
#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#include "../../ThreadPool.hpp"
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
    ZoneScoped

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

    ThreadPool::sharedPool()->queueJob([self, fullpath, eImageFormat, pathKey, plistPath, pixelFormat] {
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
            loadedImages.push_back({pImage, plistPath, pathKey, pixelFormat});
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
}

int maxProgress = 0;
class $modify(LoadingLayer) {
    void load() {
        ZoneScoped
        auto textureCache = CCTextureCache::sharedTextureCache();

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
            ZoneScopedN("load assets 7")
            addImage(textureCache, "GJ_square01.png", nullptr);
            addImage(textureCache, "GJ_square02.png", nullptr);
            addImage(textureCache, "GJ_square03.png", nullptr);
            addImage(textureCache, "GJ_square04.png", nullptr);
            addImage(textureCache, "GJ_square05.png", nullptr);
            addImage(textureCache, "gravityLine_001.png", nullptr);
        }

        // 8
        {
            ZoneScopedN("load assets 8")
            addFont(textureCache, "chatFont.fnt");

            maxProgress = ThreadPool::sharedPool()->getJobCount();
            ThreadPool::sharedPool()->finishQueue();

            // addFont doesn't work with these 2 for some reason
            CCLabelBMFont::create(" ", "goldFont.fnt");
            CCLabelBMFont::create(" ", "bigFont.fnt");
        }

        // 9
        {
            ZoneScopedN("load assets 9")

            ObjectManager::instance()->setup();
            CCAnimateFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("Robot_AnimDesc.plist");

            auto contentManager = CCContentManager::sharedManager();
            contentManager->addDict("glassDestroy01.plist", false);
            contentManager->addDict("coinPickupEffect.plist", false);
            contentManager->addDict("explodeEffect.plist", false);

            AchievementManager::sharedState();
            ObjectToolbox::sharedState();
        }

        // 10
        // it does nothing lol

        // 11
        // what.
        {
            ZoneScopedN("load assets 11")
            auto gameManager = GameManager::sharedState();
            if(gameManager->m_clickedEditor && !m_fromRefresh)
                gameManager->m_showProgressBar = true;
        }

        // 12
        {
            ZoneScopedN("load assets 12")
            CCTextInputNode::create(200.f, 50.f, "Temp", 24, "bigFont.fnt");
        }

        // 13
        {
            ZoneScopedN("load assets 13")
            if(!m_fromRefresh) {
                auto circleWave = CCCircleWave::create(10.f, 5.f, 0.3f, true, true);
                addChild(circleWave);
                circleWave->setPosition(CCPoint(-100.f, -100.f));
            }
        }
    }

    void finishLoading() {
        ZoneScopedN("load assets 14")

        auto application = CCApplication::sharedApplication();
        CCDirector::sharedDirector()->setDelegate((CCDirectorDelegate*)((uintptr_t)application + 0x98)); // idk what 98 is
        *(bool*)((uintptr_t)application + 0xa4) = true; // idk what a4 is either

        if(!m_fromRefresh)
            GameManager::sharedState()->fadeInMusic("menuLoop.mp3");

        loadingFinished();
    }

    void finishAddImage() {
        ZoneScoped
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

    void updateProgressBar() {
        ZoneScoped
        auto sliderX = m_sliderGrooveXPos;

        m_unknown2 = true;

        float progress = (1.f - (float)ThreadPool::sharedPool()->getJobCount() / maxProgress) * sliderX;
        if(progress <= sliderX)
            sliderX = progress;

        m_sliderBar->setTextureRect({ 0.f, 0.f, sliderX, m_sliderGrooveHeight });
    }

    void runLoadAssets() {
        ZoneScoped
        CCCallFunc* callFunc = CCCallFunc::create(this, callfunc_selector(LoadingLayer::loadAssets));
        CCDelayTime* delayTime = CCDelayTime::create(0.f);
        CCSequence* sequence = CCSequence::create(delayTime, callFunc, nullptr);
        GameManager::sharedState()->getActionManager()->addAction(sequence, this, false);
    }

    void loadAssets() {
        ZoneScoped
        if(m_loadStep <= 0) {
            load();
            m_loadStep = 14;
        }

        finishAddImage();

        if(ThreadPool::sharedPool()->getJobCount() > 0) {
            ZoneScopedN("wait")
            updateProgressBar();
            runLoadAssets();
        }
        else
            finishLoading();
    }
};
