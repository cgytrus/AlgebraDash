#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/LoadingLayer.hpp>

#include <shared_pool.hpp>
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
class CustomCCTextureCache : public CCTextureCache {
public:
    void addImage(const char* path, const char* plistPath, CCTexture2DPixelFormat pixelFormat = kCCTexture2DPixelFormat_DontChange) {
        ZoneScoped;

        CCImage* pImage = nullptr;

        std::string pathKey = path;
        pathKey = CCFileUtils::sharedFileUtils()->fullPathForFilename(pathKey.c_str(), false);
        if(pathKey.size() == 0)
            return;

        CCTexture2D* texture = (CCTexture2D*)m_pTextures->objectForKey(pathKey.c_str());
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

        sharedPool()->push_task([this, fullpath, eImageFormat, pathKey, plistPath, pixelFormat] {
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

    void finishAddImage() {
        ZoneScoped;
        std::unique_lock<std::mutex> lock(loadedImagesMutex);

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
                m_pTextures->setObject(texture, loadedImage.pathKey.c_str());
                if(loadedImage.plistPath)
                    spriteFrameCache->addSpriteFramesWithFile(loadedImage.plistPath, texture);
            }

            texture->release();
            CC_SAFE_RELEASE(loadedImage.image);
        }
        CCTexture2D::setDefaultAlphaPixelFormat(savedPixelFormat);

        loadedImages.clear();
    }

    void addFont(const char* fontFile) {
        ZoneScoped;
        auto fontConf = FNTConfigLoadFile(fontFile);
        if(!fontConf)
            return;
        fontConf->retain();
        addImage(fontConf->getAtlasName(), nullptr);
    }
};

struct OptimizedLoadingLayer : geode::Modify<OptimizedLoadingLayer, LoadingLayer> {
    int m_maxProgress = 0;

    void load(LoadingLayer* self) {
        ZoneScoped;
        auto textureCache = reinterpret_cast<CustomCCTextureCache*>(CCTextureCache::sharedTextureCache());

        // 1
        {
            ZoneScopedN("load assets 1");
            textureCache->addImage("GJ_GameSheet.png", "GJ_GameSheet.plist");
        }

        // 2
        {
            ZoneScopedN("load assets 2");
            textureCache->addImage("GJ_GameSheet02.png", "GJ_GameSheet02.plist");
        }

        // 3
        {
            ZoneScopedN("load assets 3");
            textureCache->addImage("GJ_GameSheet03.png", "GJ_GameSheet03.plist");
        }

        // 4
        {
            ZoneScopedN("load assets 4");
            textureCache->addImage("GJ_GameSheet04.png", "GJ_GameSheet04.plist");
        }

        // 5
        {
            ZoneScopedN("load assets 5");
            textureCache->addImage("GJ_GameSheetGlow.png", "GJ_GameSheetGlow.plist");
            textureCache->addImage("FireSheet_01.png", "FireSheet_01.plist");
            textureCache->addImage("GJ_ShopSheet.png", "GJ_ShopSheet.plist");
            textureCache->addImage("smallDot.png", nullptr);
            textureCache->addImage("square02_001.png", nullptr);
        }

        // 6
        {
            ZoneScopedN("load assets 6");

            auto useRobtopDumbFormatForColorPicker = false;//*(bool*)((uintptr_t)AppDelegate::get() + 0xa7); // TODO
            textureCache->addImage("CCControlColourPickerSpriteSheet.png", "CCControlColourPickerSpriteSheet.plist",
                useRobtopDumbFormatForColorPicker ? kCCTexture2DPixelFormat_DontChange : CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImage("GJ_gradientBG.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImage("edit_barBG_001.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImage("GJ_button_01.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImage("slidergroove2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
            textureCache->addImage("sliderBar2.png", nullptr, CCTexture2DPixelFormat::kCCTexture2DPixelFormat_Default);
        }

        // 7
        {
            ZoneScopedN("load assets 7");
            textureCache->addImage("GJ_square01.png", nullptr);
            textureCache->addImage("GJ_square02.png", nullptr);
            textureCache->addImage("GJ_square03.png", nullptr);
            textureCache->addImage("GJ_square04.png", nullptr);
            textureCache->addImage("GJ_square05.png", nullptr);
            textureCache->addImage("gravityLine_001.png", nullptr);
        }

        // 8
        {
            ZoneScopedN("load assets 8");
            textureCache->addFont("chatFont.fnt");
            textureCache->addFont("goldFont.fnt");
            textureCache->addFont("bigFont.fnt");
        }

        m_fields->m_maxProgress = sharedPool()->get_tasks_total();

        // 9
        {
            ZoneScopedN("load assets 9");

            ObjectManager::instance()->setup();

            auto animateFrameCache = CCAnimateFrameCache::sharedSpriteFrameCache();
            animateFrameCache->addSpriteFramesWithFile("Robot_AnimDesc.plist");

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
            ZoneScopedN("load assets 11");
            auto gameManager = GameManager::sharedState();
            if(gameManager->m_clickedEditor && !self->m_fromRefresh)
                gameManager->m_showProgressBar = true;
        }

        // 12
        {
            ZoneScopedN("load assets 12");
            CCTextInputNode::create(200.f, 50.f, "Temp", "bigFont.fnt");
        }

        // 13
        {
            ZoneScopedN("load assets 13");
            if(!self->m_fromRefresh) {
                auto circleWave = CCCircleWave::create(10.f, 5.f, 0.3f, true, true);
                self->addChild(circleWave);
                circleWave->setPosition(CCPoint(-100.f, -100.f));
            }
        }
    }

    void updateProgressBar() {
        ZoneScoped;
        auto sliderX = m_sliderGrooveXPos;
        float progress = (1.f - (float)sharedPool()->get_tasks_total() / m_fields->m_maxProgress) * sliderX;
        if(progress <= sliderX)
            sliderX = progress;
        m_sliderBar->setTextureRect({ 0.f, 0.f, sliderX, m_sliderGrooveHeight });
    }

    void loadAssets() {
        ZoneScoped;

        if(m_loadStep <= 0) {
            load(this);
            m_loadStep = 15;
        }

        reinterpret_cast<CustomCCTextureCache*>(CCTextureCache::sharedTextureCache())->finishAddImage();

        if(sharedPool()->get_tasks_total() > 0) {
            ZoneScopedN("wait");
            LoadingLayer::loadAssets();
            updateProgressBar();
            return;
        }
        {
            ZoneScopedN("geode assets");
            Loader::get()->updateResources(true);
        }
        {
            ZoneScopedN("load assets 14");
            m_loadStep = 14;
            LoadingLayer::loadAssets();
        }
    }

    static void onModify(auto& self) {
        // TODO
        //self.setAutoEnable(false);
        //self.setEnabled(true);
    }
};
