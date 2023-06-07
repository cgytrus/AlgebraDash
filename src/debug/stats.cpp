#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/CCDirector.hpp>
#include <Geode/modify/CCEGLViewProtocol.hpp>

class umm;
template<>
struct geode::modifier::ModifyDerive<umm, umm> {
    static void setIgnoreContentScaleFactor(CCLabelAtlas* self, bool a) {
        self->setIgnoreContentScaleFactor(a);
    }
};
struct StatsCCDirector : geode::Modify<StatsCCDirector, CCDirector> {
    void createStatsLabel() {
        CCTexture2D* texture = nullptr;
        auto textureCache = CCTextureCache::sharedTextureCache();

        if(m_pFPSLabel && m_pSPFLabel) {
            CC_SAFE_RELEASE_NULL(m_pFPSLabel);
            CC_SAFE_RELEASE_NULL(m_pSPFLabel);
            CC_SAFE_RELEASE_NULL(m_pDrawsLabel);
            textureCache->removeTextureForKey("cc_fps_images");
            CCFileUtils::sharedFileUtils()->purgeCachedEntries();
        }

        unsigned char* data = nullptr;
        unsigned int data_len = 0;
        getFPSImageData(&data, &data_len);

        CCImage* image = new CCImage();
        bool isOK = image->initWithImageData(data, data_len);
        if(!isOK) {
            log::error("Fails: init fps_images");
            return;
        }

        texture = textureCache->addUIImage(image, "cc_fps_images");
        CC_SAFE_RELEASE(image);

        /*
        We want to use an image which is stored in the file named ccFPSImage.c 
        for any design resolutions and all resource resolutions. 
        
        To achieve this,
        
        Firstly, we need to ignore 'contentScaleFactor' in 'CCAtlasNode' and 'CCLabelAtlas'.
        So I added a new method called 'setIgnoreContentScaleFactor' for 'CCAtlasNode',
        this is not exposed to game developers, it's only used for displaying FPS now.
        
        Secondly, the size of this image is 480*320, to display the FPS label with correct size, 
        a factor of design resolution ratio of 480x320 is also needed.
        */
        //float factor = CCEGLView::sharedOpenGLView()->getDesignResolutionSize().height / 320.0f;
        // its actually 2 times smaller lol
        float factor = CCEGLView::sharedOpenGLView()->getDesignResolutionSize().height / 640.0f;

        m_pFPSLabel = new CCLabelAtlas();
        geode::modifier::ModifyDerive<umm, umm>::setIgnoreContentScaleFactor(m_pFPSLabel, true);
        m_pFPSLabel->initWithString("00.0", texture, 12, 32, '.');
        m_pFPSLabel->setScale(factor);

        m_pSPFLabel = new CCLabelAtlas();
        geode::modifier::ModifyDerive<umm, umm>::setIgnoreContentScaleFactor(m_pSPFLabel, true);
        m_pSPFLabel->initWithString("0.000", texture, 12, 32, '.');
        m_pSPFLabel->setScale(factor);
        m_pSPFLabel->setVisible(false);

        m_pDrawsLabel = new CCLabelAtlas();
        geode::modifier::ModifyDerive<umm, umm>::setIgnoreContentScaleFactor(m_pDrawsLabel, true);
        m_pDrawsLabel->initWithString("000", texture, 12, 32, '.');
        m_pDrawsLabel->setScale(factor);

        //m_pDrawsLabel->setPosition(ccpAdd(ccp(0, 34 * factor), getVisibleOrigin()));
        //m_pSPFLabel->setPosition(ccpAdd(ccp(0, 17 * factor), getVisibleOrigin()));
        m_pDrawsLabel->setPosition(ccpAdd(ccp(0, 17 * factor), getVisibleOrigin()));
        m_pFPSLabel->setPosition(getVisibleOrigin());

        TracyPlotConfig("Draw Calls", tracy::PlotFormatType::Number, true, true, 0);
    }

    void setOpenGLView(CCEGLView* view) {
        bool same = m_pobOpenGLView == view;
        CCDirector::setOpenGLView(view);
        if(!same)
            createStatsLabel();
    }
    void setContentScaleFactor(float f) {
        bool same = m_fContentScaleFactor == f;
        CCDirector::setContentScaleFactor(f);
        if(!same)
            createStatsLabel();
    }

    void showStats() {
        TracyPlot("Draw Calls", static_cast<int64_t>(g_uNumberOfDraws));
        CCDirector::showStats();
    }
};
struct StatsCCEGLViewProtocol : geode::Modify<StatsCCEGLViewProtocol, CCEGLViewProtocol> {
    void setDesignResolutionSize(float width, float height, ResolutionPolicy resolutionPolicy) {
        CCEGLViewProtocol::setDesignResolutionSize(width, height, resolutionPolicy);
        log::debug("enabling stats");
        reinterpret_cast<StatsCCDirector*>(CCDirector::sharedDirector())->createStatsLabel();
        CCDirector::sharedDirector()->setDisplayStats(true);
    }
};
