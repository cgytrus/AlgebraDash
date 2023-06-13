#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCApplication.hpp>

#ifdef GEODE_IS_WINDOWS

void(__cdecl* PVRFrameEnableControlWindow)(bool);
void(__thiscall* updateConnected)(CCControllerHandler*);
$execute {
    PVRFrameEnableControlWindow = reinterpret_cast<void(__cdecl*)(bool)>(base::getCocos() + 0xc0e20);
    updateConnected = reinterpret_cast<void(__thiscall*)(CCControllerHandler*)>(base::getCocos() + 0xc7fb0);
}

class gsrdthg;
template<>
struct geode::modifier::ModifyDerive<gsrdthg, gsrdthg> {
    static void mainLoop(CCDirector* self) {
        self->mainLoop();
    }
    static bool getSmoothFixCheck(CCDirector* self) {
        return self->m_bSmoothFixCheck;
    }
    static void setActualDeltaTime(CCDirector* self, float dt) {
        self->m_fActualDeltaTime = dt;
    }
};
struct MainLoopRewrite : geode::Modify<MainLoopRewrite, CCApplication> {
    virtual int run() {
        ZoneScoped;

        PVRFrameEnableControlWindow(false);

        auto director = CCDirector::sharedDirector();
        setupGLView();
        auto view = director->getOpenGLView();
        view->retain();

        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        LARGE_INTEGER prevTime;
        QueryPerformanceCounter(&prevTime);

        if(!applicationDidFinishLaunching())
            return 0;

        int smoothFixCounter = 0;
        int noTimerFrames = 0;
        float noTimerTime = 0.f;
        m_bFullscreen = view->m_bIsFullscreen;
        bool prevFullscreen = m_bFullscreen;
        while(true) {
            if(view->windowShouldClose()) {
                ZoneScopedN("shutdown");
                if(!m_bShutdownCalled)
                    trySaveGame();
                if(view->isOpenGLReady()) {
                    director->end();
                    geode::modifier::ModifyDerive<gsrdthg, gsrdthg>::mainLoop(director);
                }
                view->release();
                platformShutdown();
                return 1;
            }

            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            long long timeDiff = currentTime.QuadPart - prevTime.QuadPart;
            if(!m_bVerticalSyncEnabled && timeDiff < m_nAnimationInterval.QuadPart)
                continue;
            ZoneScopedN("main loop frame");
            prevTime.QuadPart = currentTime.QuadPart;

            if(m_bUpdateController) {
                updateConnected(m_pControllerHandler);
                m_bUpdateController = false;
            }
            m_bControllerConnected = m_pControllerHandler->m_controllerConnected;
            if(m_bControllerConnected)
                updateControllerKeys();

            float dt = timeDiff / (float)freq.QuadPart;
            director->setDeltaTime(dt);
            geode::modifier::ModifyDerive<gsrdthg, gsrdthg>::setActualDeltaTime(director, dt);
            view->pollEvents();
            geode::modifier::ModifyDerive<gsrdthg, gsrdthg>::mainLoop(director);
        }
    }
};

#elif defined GEODE_IS_MACOS

// TODO: mac version

#endif
