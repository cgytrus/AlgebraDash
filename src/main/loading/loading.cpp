#include <Tracy.hpp>
#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#include "../../ThreadPool.hpp"
#include <mutex>

const CCTexture2DPixelFormat kCCTexture2DPixelFormat_DontChange = (CCTexture2DPixelFormat)-1;

class $modify(CCThreadedTextureCache, CCTextureCache) {
    struct LoadedImage {
        CCImage* image;
        const char* plistPath;
        std::string pathKey;
        CCTexture2DPixelFormat pixelFormat;
    };
    field<std::mutex> loadedImagesMutex;
    field<std::vector<LoadedImage>> loadedImages;
    void addImageThread(const char* path, const char* plistPath, CCTexture2DPixelFormat pixelFormat = kCCTexture2DPixelFormat_DontChange) {
        ZoneScoped

        CCImage* pImage = nullptr;

        std::string pathKey = path;
        pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str(), false);
        if(pathKey.size() == 0)
            return;

        CCTexture2D* texture = (CCTexture2D*)m_pTextures->objectForKey(pathKey.c_str());
        if(texture) {
            {
                std::unique_lock<std::mutex> lock(this->*loadedImagesMutex);
                (this->*loadedImages).push_back({pImage, plistPath, pathKey, texture->getPixelFormat()});
            }
            return;
        }

        std::string fullpath = pathKey;
        std::string lowerCase(pathKey);
        for(unsigned int i = 0; i < lowerCase.length(); ++i)
            lowerCase[i] = tolower(lowerCase[i]);

        if(std::string::npos != lowerCase.find(".pvr")) {
            addPVRImage(fullpath.c_str());
            return;
        }

        if(std::string::npos != lowerCase.find(".pkm")) {
            addETCImage(fullpath.c_str());
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

        ThreadPool::sharedPool()->queueJob([this, fullpath, eImageFormat, pathKey, plistPath, pixelFormat] {
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
                std::unique_lock<std::mutex> lock(this->*loadedImagesMutex);
                (this->*loadedImages).push_back({pImage, plistPath, pathKey, pixelFormat});
            }
        });
    }

    void addFontThread(const char* fontFile) {
        ZoneScoped
        auto fontConf = FNTConfigLoadFile(fontFile);
        if(!fontConf)
            return;
        fontConf->retain();
        addImageThread(fontConf->getAtlasName(), nullptr);
    }

    void finishAddImage() {
        ZoneScoped
        std::unique_lock<std::mutex> lock(this->*loadedImagesMutex);

        auto spriteFrameCache = CCSpriteFrameCache::sharedSpriteFrameCache();

        auto savedPixelFormat = CCTexture2D::defaultAlphaPixelFormat();
        for(auto loadedImage : this->*loadedImages) {
            auto texture = new CCTexture2D();
            if(!texture) {
                CC_SAFE_RELEASE(loadedImage.image);
                continue;
            }

            CCTexture2D::setDefaultAlphaPixelFormat(loadedImage.pixelFormat < 0 ? savedPixelFormat : loadedImage.pixelFormat);
            if(texture->initWithImage(loadedImage.image)) {
                m_pTextures->setObject(texture, loadedImage.pathKey.c_str());
                if(loadedImage.plistPath)
                    spriteFrameCache->addSpriteFramesWithFile(loadedImage.plistPath, texture);
            }

            texture->release();
            CC_SAFE_RELEASE(loadedImage.image);
        }
        CCTexture2D::setDefaultAlphaPixelFormat(savedPixelFormat);

        (this->*loadedImages).clear();
    }
};

int maxProgress = 0;
class $modify(LoadingLayer) {
    void load() {
        ZoneScoped
        auto textureCache = reinterpret_cast<CCThreadedTextureCache*>(CCTextureCache::sharedTextureCache());

        // 1
        {
            ZoneScopedN("load assets 1")
            textureCache->addImageThread("GJ_GameSheet.png", "GJ_GameSheet.plist");
        }

        // 2
        {
            ZoneScopedN("load assets 2")
            textureCache->addImageThread("GJ_GameSheet02.png", "GJ_GameSheet02.plist");
        }

        // 3
        {
            ZoneScopedN("load assets 3")
            textureCache->addImageThread("GJ_GameSheet03.png", "GJ_GameSheet03.plist");
        }

        // 4
        {
            ZoneScopedN("load assets 4")
            textureCache->addImageThread("GJ_GameSheet04.png", "GJ_GameSheet04.plist");
        }

        // 5
        {
            ZoneScopedN("load assets 5")
            textureCache->addImageThread("GJ_GameSheetGlow.png", "GJ_GameSheetGlow.plist");
            textureCache->addImageThread("FireSheet_01.png", "FireSheet_01.plist");
            textureCache->addImageThread("GJ_ShopSheet.png", "GJ_ShopSheet.plist");
            textureCache->addImageThread("smallDot.png", nullptr);
            textureCache->addImageThread("square02_001.png", nullptr);
        }

        // 6
        {
            ZoneScopedN("load assets 6")
            auto useRobtopDumbFormatForColorPicker = *(bool*)((uintptr_t)CCApplication::sharedApplication() + 0xa7);
            textureCache->addImageThread("CCControlColourPickerSpriteSheet.png", "CCControlColourPickerSpriteSheet.plist",
                useRobtopDumbFormatForColorPicker ? kCCTexture2DPixelFormat_DontChange : CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImageThread("GJ_gradientBG.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImageThread("edit_barBG_001.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImageThread("GJ_button_01.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImageThread("slidergroove2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImageThread("sliderBar2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        }

        // 7
        {
            ZoneScopedN("load assets 7")
            textureCache->addImageThread("GJ_square01.png", nullptr);
            textureCache->addImageThread("GJ_square02.png", nullptr);
            textureCache->addImageThread("GJ_square03.png", nullptr);
            textureCache->addImageThread("GJ_square04.png", nullptr);
            textureCache->addImageThread("GJ_square05.png", nullptr);
            textureCache->addImageThread("gravityLine_001.png", nullptr);
        }

        // 8
        {
            ZoneScopedN("load assets 8")
            textureCache->addFontThread("chatFont.fnt");
            textureCache->addFontThread("goldFont.fnt");
            textureCache->addFontThread("bigFont.fnt");
        }

        maxProgress = ThreadPool::sharedPool()->getJobCount();
        ThreadPool::sharedPool()->finishQueue();

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

// it doesn't work on mac so
// (cuz hardcoded offsets)
#ifdef GEODE_IS_WINDOWS
    void loadAssets() {
        ZoneScoped
        if(m_loadStep <= 0) {
            load();
            m_loadStep = 14;
        }

        reinterpret_cast<CCThreadedTextureCache*>(CCTextureCache::sharedTextureCache())->finishAddImage();

        if(ThreadPool::sharedPool()->getJobCount() > 0) {
            ZoneScopedN("wait")
            updateProgressBar();
            runLoadAssets();
        }
        else
            finishLoading();
    }
#endif
};
