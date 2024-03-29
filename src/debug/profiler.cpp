#ifdef TRACY_ENABLE

#include <Tracy.hpp>
#include <TracyOpenGL.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/Modify.hpp>

struct MarkFrame : geode::Modify<MarkFrame, CCEGLView> {
    bool initGlew() {
        ZoneScopedN("CCEGLView::initGlew");
        if(!CCEGLView::initGlew())
            return false;
        TracyGpuContext;
        return true;
    }

    void swapBuffers() {
        ZoneScopedN("CCEGLView::swapBuffers");
        CCEGLView::swapBuffers();
        // TODO: this crashes??????
        //TracyGpuCollect;
        FrameMark;
    }
};

void memoryAllocated(void* memory, size_t size) {
    TracyAlloc(memory, size);
}
void memoryFreed(void* memory) {
    TracyFree(memory);
}
void* __cdecl operatorNewHook(size_t size) {
    auto ret = operator new(size);
    memoryAllocated(ret, size);
    return ret;
}
void __cdecl operatorDeleteHook(void* memory) {
    memoryFreed(memory);
    operator delete(memory);
}
void __cdecl operatorDeleteArrayHook(void* memory) {
    memoryFreed(memory);
    operator delete[](memory);
}
void* __cdecl mallocHook(size_t size) {
    auto ret = malloc(size);
    memoryAllocated(ret, size);
    return ret;
}
void* __cdecl callocHook(size_t count, size_t size) {
    auto ret = calloc(count, size);
    memoryAllocated(ret, size * count);
    return ret;
}
void __cdecl freeHook(void* memory) {
    memoryFreed(memory);
    free(memory);
}
void* __cdecl reallocHook(void* memory, size_t newSize) {
    memoryFreed(memory);
    auto ret = realloc(memory, newSize);
    memoryAllocated(ret, newSize);
    return ret;
}
$execute {
    bool success = true;

    auto h = GetModuleHandle("msvcr120.dll");
    success = success && Mod::get()->addHook(reinterpret_cast<void*>(GetProcAddress(h, "realloc")), &reallocHook, "realloc", tulip::hook::TulipConvention::Cdecl).isOk();
    success = success && Mod::get()->addHook(reinterpret_cast<void*>(GetProcAddress(h, "calloc")), &callocHook, "calloc", tulip::hook::TulipConvention::Cdecl).isOk();
    success = success && Mod::get()->addHook(reinterpret_cast<void*>(GetProcAddress(h, "malloc")), &mallocHook, "malloc", tulip::hook::TulipConvention::Cdecl).isOk();
    success = success && Mod::get()->addHook(reinterpret_cast<void*>(GetProcAddress(h, "free")), &freeHook, "free", tulip::hook::TulipConvention::Cdecl).isOk();

    if(!success)
        log::error("failed to hook some memory stuff");
}

#define PROFILER_HOOK_3(Ret_, Class_, Name_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_() { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(); } };
#define PROFILER_HOOK_4(Ret_, Class_, Name_, A_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a); } };
#define PROFILER_HOOK_5(Ret_, Class_, Name_, A_, B_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b); } };
#define PROFILER_HOOK_6(Ret_, Class_, Name_, A_, B_, C_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c); } };
#define PROFILER_HOOK_7(Ret_, Class_, Name_, A_, B_, C_, D_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d); } };
#define PROFILER_HOOK_8(Ret_, Class_, Name_, A_, B_, C_, D_, E_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d, E_ e) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d, e); } };
#define PROFILER_HOOK_9(Ret_, Class_, Name_, A_, B_, C_, D_, E_, F_) class GEODE_CRTP2(GEODE_CONCAT(profilerHook, __LINE__), Class_) { \
    Ret_ Name_(A_ a, B_ b, C_ c, D_ d, E_ e, F_ f) { ZoneScopedN(#Class_ "::" #Name_); return Class_::Name_(a, b, c, d, e, f); } };
#define PROFILER_HOOK(...) GEODE_INVOKE(GEODE_CONCAT(PROFILER_HOOK_, GEODE_NUMBER_OF_ARGS(__VA_ARGS__)), __VA_ARGS__)

// haha rip dll size

// pre frame
PROFILER_HOOK(void, CCEGLView, pollEvents)
  //PROFILER_HOOK(void, CCEGLView, FUN_10110dc0, int)
  //PROFILER_HOOK(void, CCEGLView, FUN_10111a80, int, int, int, int, int)
PROFILER_HOOK(void, CCDirector, setDeltaTime, float)

// actual frame
PROFILER_HOOK(void, CCDirector, drawScene)
PROFILER_HOOK(void, CCDirector, setNextScene)

// gd init
PROFILER_HOOK(bool, DrawGridLayer, init, CCNode*, LevelEditorLayer*)
PROFILER_HOOK(bool, EditLevelLayer, init, GJGameLevel*)
PROFILER_HOOK(bool, EditorUI, init, LevelEditorLayer*)
PROFILER_HOOK(bool, LevelEditorLayer, init, GJGameLevel*)
PROFILER_HOOK(bool, LoadingLayer, init, bool)
PROFILER_HOOK(bool, MenuLayer, init)
PROFILER_HOOK(bool, ObjectToolbox, init)
PROFILER_HOOK(bool, PlayerObject, init, int, int, CCLayer*)
PROFILER_HOOK(bool, PlayLayer, init, GJGameLevel*)

// gd update
PROFILER_HOOK(void, DrawGridLayer, update, float)
PROFILER_HOOK(void, FMODAudioEngine, update, float)
PROFILER_HOOK(void, GameManager, update, float)
PROFILER_HOOK(void, LevelEditorLayer, update, float)
PROFILER_HOOK(void, PlayerObject, update, float)
PROFILER_HOOK(void, PlayLayer, update, float)

PROFILER_HOOK(void, GJEffectManager, updateColorEffects, float)
PROFILER_HOOK(void, GJEffectManager, updatePulseEffects, float)
PROFILER_HOOK(void, GJEffectManager, updateOpacityEffects, float)
PROFILER_HOOK(void, GJEffectManager, updateSpawnTriggers, float)
PROFILER_HOOK(void, GJEffectManager, preCollisionCheck)
PROFILER_HOOK(void, GJEffectManager, postCollisionCheck)
PROFILER_HOOK(void, GJEffectManager, prepareMoveActions, float, bool)
PROFILER_HOOK(void, GJBaseGameLayer, processMoveActionsStep, float)
PROFILER_HOOK(void, GJBaseGameLayer, processRotationActions)
PROFILER_HOOK(void, GJBaseGameLayer, updateDisabledObjectsLastPos, CCArray*)
PROFILER_HOOK(void, GJBaseGameLayer, processMoveActions)
PROFILER_HOOK(void, GJBaseGameLayer, processPlayerFollowActions, float)
PROFILER_HOOK(void, PlayLayer, processFollowActions)
PROFILER_HOOK(void, PlayLayer, updateCollisionBlocks)
PROFILER_HOOK(void, PlayerObject, updateCheckpointTest)
PROFILER_HOOK(void, PlayLayer, updateQueuedLabels)
PROFILER_HOOK(void, PlayLayer, updateCamera, float)
PROFILER_HOOK(void, PlayLayer, updateVisibility)
PROFILER_HOOK(void, PlayLayer, updateLevelColors)
PROFILER_HOOK(void, PlayLayer, updateProgressbar)
PROFILER_HOOK(void, PlayLayer, updateEffectPositions)
PROFILER_HOOK(void, GJEffectManager, calculateBaseActiveColors)
PROFILER_HOOK(void, GJEffectManager, processPulseActions)
PROFILER_HOOK(void, GJEffectManager, processInheritedColors)
PROFILER_HOOK(void, GJEffectManager, processCopyColorPulseActions)
PROFILER_HOOK(const ccColor3B&, GJEffectManager, getColorSprite, int)
PROFILER_HOOK(void, GJEffectManager, calculateLightBGColor, ccColor3B)
 // TODO what // PROFILER_HOOK(void, GameToolbox, transformColor, ccColor3B*, ccColor3B*, ccHSVValue)
 // TODO what // PROFILER_HOOK(float, GameObject, groupOpacityMod)
PROFILER_HOOK(void, GameObject, addColorSpriteToParent, bool)
PROFILER_HOOK(void, AnimatedGameObject, updateChildSpriteColor, ccColor3B)
PROFILER_HOOK(ccColor3B&, GameObject, colorForMode, int, bool)
PROFILER_HOOK(ccColor3B&, GameObject, groupColor, const ccColor3B&, bool)
PROFILER_HOOK(ccColor3B&, GameObject, getActiveColorForMode, int, bool)
PROFILER_HOOK(void, GameObject, addMainSpriteToParent, bool)
PROFILER_HOOK(void, GameObject, addColorSpriteToParent, bool)
PROFILER_HOOK(void, GameObject, setObjectColor, const ccColor3B&)

PROFILER_HOOK(void, GameObject, updateOrientedBox)
PROFILER_HOOK(void, GJBaseGameLayer, reorderObjectSection, GameObject*)
 // TODO address // PROFILER_HOOK(void, std::vector<GameObject*>, emplace, GameObject*)
PROFILER_HOOK(void, GJBaseGameLayer, removeObjectFromSection, GameObject*)

PROFILER_HOOK(void, GameObject, activateObject)
 // TODO address // PROFILER_HOOK(void, GameObject, activateObject, PlayerObject*)
PROFILER_HOOK(void, GameObject, deactivateObject, bool)

PROFILER_HOOK(bool, GameManager, getGameVariable, const char*)
PROFILER_HOOK(void, GameManager, setGameVariable, const char*, bool)

// gd draw
PROFILER_HOOK(void, DrawGridLayer, draw)
PROFILER_HOOK(void, EditorUI, draw)
PROFILER_HOOK(void, LevelEditorLayer, draw)

// loading
PROFILER_HOOK(void, LoadingLayer, loadAssets)
PROFILER_HOOK(void, LoadingLayer, loadingFinished)
PROFILER_HOOK(CCTexture2D*, CCTextureCache, addImage, const char*, bool)
PROFILER_HOOK(void, CCTextureCache, addImageAsync, const char*, CCObject*, SEL_CallFuncO)
 // TODO address private // PROFILER_HOOK(void*, CCTextureCache, loadImage)
 // TODO address private // PROFILER_HOOK(void, CCTextureCache, loadImageData)
 // TODO private // PROFILER_HOOK(void, CCSpriteFrameCache, addSpriteFramesWithDictionary, CCDictionary*, CCTexture2D*)
PROFILER_HOOK(void, CCSpriteFrameCache, addSpriteFramesWithFile, const char*, const char*)
PROFILER_HOOK(void, CCSpriteFrameCache, addSpriteFramesWithFile, const char*)
PROFILER_HOOK(void, CCSpriteFrameCache, addSpriteFramesWithFile, const char*, CCTexture2D*)
PROFILER_HOOK(CCLabelBMFont*, CCLabelBMFont, create, const char*, const char*, float, CCTextAlignment, CCPoint)
PROFILER_HOOK(void, GameSoundManager, asynchronousSetup)
 // TODO address 0xc9420 // PROFILER_HOOK(void, GameManager, FUN_004c9420)
 // TODO address 0xa8cb0 // PROFILER_HOOK(void, GameLevelManager, FUN_004a8cb0)
PROFILER_HOOK(bool, CCTexture2D, initWithImage, CCImage*)
PROFILER_HOOK(CCBMFontConfiguration*, CCBMFontConfiguration, create, const char*)
 // TODO private // PROFILER_HOOK(void*, CCBMFontConfiguration, parseConfigFile, const char*)
PROFILER_HOOK(bool, CCSpriteBatchNode, initWithTexture, CCTexture2D*, unsigned int)
PROFILER_HOOK(bool, CCSprite, initWithTexture, CCTexture2D*, const CCRect&, bool)
 // TODO address // PROFILER_HOOK(bool, CCTextureAtlas, initWithTexture, CCTexture2D*, unsigned int)
 // TODO private // PROFILER_HOOK(void, CCTextureAtlas, setupIndices)
 // TODO private // PROFILER_HOOK(void, CCTextureAtlas, mapBuffers)

PROFILER_HOOK(void, CCScheduler, update, float)
 // TODO address // PROFILER_HOOK(void, CCAccelerometer, update, double, double, double, double)
 // TODO address // PROFILER_HOOK(void, CCTimer, update, float)

// actions
PROFILER_HOOK(void, CCActionManager, update, float)
 // TODO address // PROFILER_HOOK(void, CCTargetedAction, update, float)
 // TODO address // PROFILER_HOOK(void, CCActionEase, update, float)
PROFILER_HOOK(void, CCActionTween, update, float)
PROFILER_HOOK(void, CCAnimate, update, float)
PROFILER_HOOK(void, CCCallFunc, update, float)
PROFILER_HOOK(void, CCSequence, update, float)

 // TODO address // PROFILER_HOOK(void, CCBezierBy, update, float)
 // TODO address // PROFILER_HOOK(void, CCBlink, update, float)
 // TODO address // PROFILER_HOOK(void, CCCardinalSplineTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCFadeIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCFadeOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCFadeOutTRTiles, update, float)
 // TODO address // PROFILER_HOOK(void, CCFadeTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCFlipX, update, float)
 // TODO address // PROFILER_HOOK(void, CCFlipY, update, float)
 // TODO address // PROFILER_HOOK(void, CCJumpBy, update, float)
 // TODO address // PROFILER_HOOK(void, CCMoveBy, update, float)
 // TODO address // PROFILER_HOOK(void, CCPlace, update, float)
 // TODO address // PROFILER_HOOK(void, CCRemoveSelf, update, float)
 // TODO address // PROFILER_HOOK(void, CCRepeat, update, float)
 // TODO address // PROFILER_HOOK(void, CCReverseTime, update, float)
 // TODO address // PROFILER_HOOK(void, CCRotateBy, update, float)
 // TODO address // PROFILER_HOOK(void, CCRotateTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCScaleTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCSpawn, update, float)
 // TODO address // PROFILER_HOOK(void, CCTintBy, update, float)
 // TODO address // PROFILER_HOOK(void, CCTintTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCToggleVisibility, update, float)
 // TODO address // PROFILER_HOOK(void, CCHide, update, float)
 // TODO address // PROFILER_HOOK(void, CCShow, update, float)

// progress
 // TODO address // PROFILER_HOOK(void, CCProgressFromTo, update, float)
 // TODO address // PROFILER_HOOK(void, CCProgressTimer, updateBar)
 // TODO address // PROFILER_HOOK(void, CCProgressTimer, updateColor)
 // TODO address // PROFILER_HOOK(void, CCProgressTimer, updateProgress)
 // TODO address // PROFILER_HOOK(void, CCProgressTimer, updateRadial)

// easings
 // TODO address // PROFILER_HOOK(void, CCEaseBackIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseBackInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseBackOut, update, float)

 // TODO address // PROFILER_HOOK(void, CCEaseBounceIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseBounceInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseBounceOut, update, float)

 // TODO address // PROFILER_HOOK(void, CCEaseElasticIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseElasticInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseElasticOut, update, float)

 // TODO address // PROFILER_HOOK(void, CCEaseExponentialIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseExponentialInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseExponentialOut, update, float)

 // TODO address // PROFILER_HOOK(void, CCEaseIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseOut, update, float)

 // TODO address // PROFILER_HOOK(void, CCEaseSineIn, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseSineInOut, update, float)
 // TODO address // PROFILER_HOOK(void, CCEaseSineOut, update, float)

// particles
 // TODO address // PROFILER_HOOK(void, CCParticleBatchNode, draw)
 // TODO address // PROFILER_HOOK(void, CCParticleBatchNode, visit)
PROFILER_HOOK(void, CCParticleSystem, update, float)
PROFILER_HOOK(void, CCParticleSystem, updateWithNoTime)
PROFILER_HOOK(void, CCParticleSystemQuad, draw)

// sprites
PROFILER_HOOK(void, CCSprite, setOpacity, GLubyte)
PROFILER_HOOK(void, CCSprite, setColor, const ccColor3B&)
PROFILER_HOOK(void, CCSprite, setOpacityModifyRGB, bool)
PROFILER_HOOK(void, CCSprite, updateDisplayedColor, const ccColor3B&)
PROFILER_HOOK(void, CCSprite, updateDisplayedOpacity, GLubyte)
PROFILER_HOOK(void, CCSprite, updateColor)
PROFILER_HOOK(void, CCSprite, updateTransform)
PROFILER_HOOK(void, CCSprite, draw)
PROFILER_HOOK(CCSpriteFrame*, CCSprite, displayFrame)
PROFILER_HOOK(void, CCSprite, sortAllChildren)
PROFILER_HOOK(void, CCSpriteBatchNode, draw)
PROFILER_HOOK(void, CCSpriteBatchNode, sortAllChildren)
 // TODO private // PROFILER_HOOK(void, CCSpriteBatchNode, updateAtlastIndex, CCSprite*, int*)
 // TODO private // PROFILER_HOOK(void, CCSpriteBatchNode, swap, int, int)

PROFILER_HOOK(void, CCLabelBMFont, setString, const char*, bool)
PROFILER_HOOK(void, CCLabelBMFont, updateLabel)

 // TODO address // PROFILER_HOOK(void, CCTextureAtlas, updateQuad, ccV3F_C4B_T2F_Quad*, unsigned int)

PROFILER_HOOK(void, CCNode, update, float)
PROFILER_HOOK(void, CCNode, visit)
PROFILER_HOOK(void, CCNode, updateTransform)

// post frame? haven't actually seen this getting called so idk
 // TODO address // PROFILER_HOOK(void, CCApplication, updateVerticalSync)
 // TODO address // PROFILER_HOOK(void, CCApplication, updateControllerKeys)

#endif
