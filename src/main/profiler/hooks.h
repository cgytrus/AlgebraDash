#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#define AD_CONCAT(x, y) x##y
#define AD_COMMENT AD_CONCAT(/, /)

// temporary defines to specify why smth isn't working
#ifdef GEODE_IS_WINDOWS
#define MISSING_ON_WINDOWS AD_COMMENT
#define MISSING_ON_MAC
#else
#define MISSING_ON_WINDOWS
#define MISSING_ON_MAC AD_COMMENT
#endif

#define MISSING_ON_BOTH AD_COMMENT
#define MISSING_ON_MAC_COCOS2D AD_COMMENT
#define MISSING_IN_COCOS2D_NOT_A_MEMBER AD_COMMENT // ???????
#define MISSING_IN_COCOS2D_PRIVATE AD_COMMENT
#define MISSING_IN_COCOS2D_CRASHES AD_COMMENT // ???????????

#define TODO_HOOK_ADDRESS AD_COMMENT

// hook defines
#define PROFILER_HOOK_AUTO_ARGS(ret, type, name, argsC, ...) PROFILER_HOOK(ret, type, name, argsC, (__VA_ARGS__))
#define PROFILER_HOOK_AUTO(ret, type, name) PROFILER_HOOK(ret, type, name, (), ())

// haha rip dll size

// pre frame
PROFILER_HOOK_AUTO(void, cocos2d::CCEGLView, pollEvents)
TODO_HOOK_ADDRESS PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x110dc0, void, __cdecl, cocos2d::CCEGLView, FUN_10110dc0, (idk), int idk)
TODO_HOOK_ADDRESS PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x111a80, void, __cdecl, cocos2d::CCEGLView, FUN_10111a80, (idk1, idk2, idk3, idk4, idk5), int idk1, int idk2, int idk3, int idk4, int idk5)
PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCDirector, setDeltaTime, (dt), float dt)

// actual frame
PROFILER_HOOK_AUTO(void, cocos2d::CCDirector, drawScene)
PROFILER_HOOK_AUTO(void, cocos2d::CCDirector, setNextScene)

// gd init
MISSING_ON_MAC PROFILER_HOOK_AUTO_ARGS(bool, DrawGridLayer, init, (grid, editor), CCNode* grid, LevelEditorLayer* editor) // win 0x16c4d0 // pred
MISSING_ON_MAC PROFILER_HOOK_AUTO_ARGS(bool, EditLevelLayer, init, (level), GJGameLevel* level) // win 0x6f5d0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(bool, EditorUI, init, (level), LevelEditorLayer* level) // win 0x76310 // pred
PROFILER_HOOK_AUTO_ARGS(bool, LevelEditorLayer, init, (level), GJGameLevel* level)
PROFILER_HOOK_AUTO_ARGS(bool, LoadingLayer, init, (fromReload), bool fromReload)
PROFILER_HOOK_AUTO(bool, MenuLayer, init)
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(bool, ObjectToolbox, init) // win 0x198b20 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(bool, PlayerObject, init, (idk1, idk2, idk3), int idk1, int idk2, CCLayer* idk3) // win 0x1e6da0 // pred
PROFILER_HOOK_AUTO_ARGS(bool, PlayLayer, init, (level), GJGameLevel* level)

// gd update
MISSING_ON_MAC PROFILER_HOOK_AUTO_ARGS(void, DrawGridLayer, update, (dt), float dt) // win 0x16cd80 // pred
MISSING_ON_MAC PROFILER_HOOK_AUTO_ARGS(void, FMODAudioEngine, update, (dt), float dt) // win 0x23b20 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GameManager, update, (dt), float dt) // win 0xce440 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, LevelEditorLayer, update, (dt), float dt) // win 0x26a660 in geode (should be 0x16a660) // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, PlayerObject, update, (dt), float dt) // win 0x1e8200 // pred
PROFILER_HOOK_AUTO_ARGS(void, PlayLayer, update, (dt), float dt)

/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, updateColorEffects, (dt), float dt) // win 0x11e1d0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, updatePulseEffects, (dt), float dt) // win 0x11e7f0 // pred
PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, updateOpacityEffects, (dt), float dt)
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, updateSpawnTriggers, (dt), float dt) // win 0x11f000 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, preCollisionCheck) // win 0x11d030 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, prepareMoveActions, (idk1, idk2), float idk1, bool idk2) // win 0x11da30 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJBaseGameLayer, processMoveActionsStep, (dt), float dt) // win 0x10ffa0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJBaseGameLayer, processRotationActions) // win 0x1101a0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJBaseGameLayer, processMoveActions) // win 0x1107e0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJBaseGameLayer, processPlayerFollowActions, (dt), float dt) // win 0x110b10 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJBaseGameLayer, processFollowActions) // win 0x110e00 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJBaseGameLayer, updateCollisionBlocks) // win 0x10ef70 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, PlayerObject, updateCheckpointTest) // win 0x1e8b50 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, postCollisionCheck) // win 0x11d0b0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJBaseGameLayer, updateQueuedLabels) // win 0x111b00 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, PlayLayer, updateCamera, (dt), float dt) // win 0x2071d0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, PlayLayer, loadDefaultColors) // win 0x206e10 // pred
PROFILER_HOOK_AUTO(void, PlayLayer, updateVisibility)
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, calculateBaseActiveColors) // win 0x11c7c0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, processPulseActions) // win 0x11ea50 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, processInheritedColors) // win 0x11c8a0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GJEffectManager, processCopyColorPulseActions) // win 0x11ebc0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(const ccColor3B&, GJEffectManager, getColorSprite, (index), int index) // win 0x11ce20 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJEffectManager, calculateLightBGColor, (color3), ccColor3B color3) // win 0x11f420 // pred
//PROFILER_HOOK_AUTO_ARGS(static void, GameToolbox, transformColor, (src, dest, hsv), ccColor3B* src, ccColor3B* dest, ccHSVValue hsv) // crashes idk
TODO_HOOK_ADDRESS PROFILER_HOOK_THIS(GD + 0xebda0, void, GameObject, FUN_004ebda0)
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GameObject, addColorSpriteToParent, (idk), bool idk) // win 0xeb3f0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, AnimatedGameObject, updateChildSpriteColor, (color3), ccColor3B color3) // win 0x2531f0 // pred
PROFILER_HOOK_AUTO_ARGS(void, GameObject, setObjectColor, (color), const ccColor3B& color)

PROFILER_HOOK_AUTO(void, GameObject, updateOrientedBox)
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJBaseGameLayer, reorderObjectSection, (obj), GameObject* obj) // win 0x10fe10 // pred
MISSING_ON_BOTH PROFILER_HOOK_AUTO_ARGS(void, std::vector<GameObject*>, emplace, (elem), GameObject* elem) // win 0x112470 // not sure if it should be in geode
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO_ARGS(void, GJBaseGameLayer, removeObjectFromSection, (obj), GameObject* obj) // win 0x10ff30 // pred

/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, GameObject, activateObject) // wrong overload in geode on win, should be win 0xd1870 instead of 0xef0e0 // pred
MISSING_ON_MAC PROFILER_HOOK_AUTO_ARGS(void, GameObject, activateObject, (player), PlayerObject* player) // win 0xef0e0 // pred
PROFILER_HOOK_AUTO_ARGS(void, GameObject, deactivateObject, (idk), bool idk)

PROFILER_HOOK_AUTO_ARGS(bool, GameManager, getGameVariable, (key), const char* key)
PROFILER_HOOK_AUTO_ARGS(void, GameManager, setGameVariable, (key, value), const char* key, bool value)

// gd draw
MISSING_ON_MAC PROFILER_HOOK_AUTO(void, DrawGridLayer, draw)
MISSING_ON_MAC PROFILER_HOOK_AUTO(void, EditorUI, draw) // win 0x18fbe0 // pred
/* MISSING_ON_WINDOWS */ PROFILER_HOOK_AUTO(void, LevelEditorLayer, draw) // win 0x16b7c0 // pred

// loading
PROFILER_HOOK_AUTO(void, LoadingLayer, loadAssets)
PROFILER_HOOK_AUTO_ARGS(CCTexture2D*, cocos2d::CCTextureCache, addImage, (path, idk), const char* path, bool idk)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTextureCache, addImageAsync, (path, target, selector), const char* path, CCObject* target, SEL_CallFuncO selector)
TODO_HOOK_ADDRESS PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec7c0, void*, __cdecl, cocos2d::CCTextureCache, loadImage, (data), void* data)
TODO_HOOK_ADDRESS PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec9a0, void, __cdecl, cocos2d::CCTextureCache, loadImageData, (pAsyncStruct), void* pAsyncStruct)
MISSING_IN_COCOS2D_PRIVATE PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithDictionary, (dict, texture), CCDictionary* dict, CCTexture2D* texture)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath, texturePath), const char* plistPath, const char* texturePath)
PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath), const char* plistPath)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath, texture), const char* plistPath, CCTexture2D* texture)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(static CCLabelBMFont*, cocos2d::CCLabelBMFont, create, (idk1, idk2, idk3, idk4, idk5), const char* idk1, const char* idk2, float idk3, CCTextAlignment idk4, CCPoint idk5)
MISSING_ON_MAC PROFILER_HOOK_AUTO(void, GameSoundManager, asynchronousSetup) // win 0x25520 // pred
TODO_HOOK_ADDRESS PROFILER_HOOK_AUTO(GD + 0xc9420, void, GameManager, FUN_0049420)
TODO_HOOK_ADDRESS PROFILER_HOOK_AUTO(GD + 0xa8cb0, void, GameLevelManager, FUN_004a8cb0)
PROFILER_HOOK_AUTO_ARGS(bool, cocos2d::CCTexture2D, initWithImage, (image), CCImage* image)
PROFILER_HOOK_AUTO_ARGS(static CCBMFontConfiguration*, cocos2d::CCBMFontConfiguration, create, (fntFile), const char* fntFile)
MISSING_IN_COCOS2D_PRIVATE PROFILER_HOOK_AUTO_ARGS(void*, cocos2d::CCBMFontConfiguration, parseConfigFile, (fntFile), const char* fntFile)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(bool, cocos2d::CCSpriteBatchNode, initWithTexture, (texture, capacity), CCTexture2D* texture, unsigned int capacity)
PROFILER_HOOK_AUTO_ARGS(bool, cocos2d::CCSprite, initWithTexture, (texture, rect, rotated), CCTexture2D* texture, const CCRect& rect, bool rotated)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(bool, cocos2d::CCTextureAtlas, initWithTexture, (texture, capacity), CCTexture2D* texture, unsigned int capacity)
MISSING_IN_COCOS2D_PRIVATE PROFILER_HOOK_AUTO(void, cocos2d::CCTextureAtlas, setupIndices)
MISSING_IN_COCOS2D_PRIVATE PROFILER_HOOK_AUTO(void, cocos2d::CCTextureAtlas, mapBuffers)

PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCScheduler, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCAccelerometer, update, (idk1, idk2, idk3, idk4), double idk1, double idk2, double idk3, double idk4)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTimer, update, (dt), float dt)

// actions
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCActionManager, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTargetedAction, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCActionEase, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCActionTween, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCAnimate, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCCallFunc, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSequence, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCBezierBy, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCBlink, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCCardinalSplineTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFadeIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFadeOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFadeOutTRTiles, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFadeTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFlipX, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCFlipY, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCJumpBy, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCMoveBy, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCPlace, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCRemove, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCRepeat, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCReverseTime, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCRotateBy, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCRotateTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCScaleTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpawn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTintBy, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTintTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCToggleVisibility, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCHide, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCShow, update, (dt), float dt)

// progress
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCProgressFromTo, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCProgressTimer, updateBar)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCProgressTimer, updateColor)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCProgressTimer, updateProgress)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCProgressTimer, updateRadial)

// easings
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBackIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBackInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBackOut, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBounceIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBounceInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseBounceOut, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseElasticIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseElasticInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseElasticOut, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseExponentialIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseExponentialInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseExponentialOut, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseOut, update, (dt), float dt)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseSineIn, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseSineInOut, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCEaseSineOut, update, (dt), float dt)

// particles
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCParticleBatchNode, draw)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCParticleBatchNode, visit)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCParticleSystem, update, (dt), float dt)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCParticleSystem, updateWithNoTime)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCParticleSystemQuad, draw)

// sprites
MISSING_IN_COCOS2D_CRASHES PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSprite, setOpacity, (opacity), GLubyte opacity)
MISSING_IN_COCOS2D_CRASHES PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSprite, setColor, (color3), const ccColor3B& color3)
MISSING_IN_COCOS2D_CRASHES PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSprite, setOpacityModifyRGB, (modify), bool modify)
MISSING_IN_COCOS2D_CRASHES PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSprite, updateDisplayedColor, (parentColor), const ccColor3B& parentColor)
MISSING_IN_COCOS2D_CRASHES PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSprite, updateDisplayedOpacity, (opacity), GLubyte opacity)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCSprite, updateColor)
PROFILER_HOOK_AUTO(void, cocos2d::CCSprite, updateTransform)
PROFILER_HOOK_AUTO(void, cocos2d::CCSprite, draw)
PROFILER_HOOK_AUTO(CCSpriteFrame*, cocos2d::CCSprite, displayFrame)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCSpriteBatchNode, draw)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCSpriteBatchNode, sortAllChildren)
MISSING_IN_COCOS2D_NOT_A_MEMBER PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteBatchNode, updateAtlastIndex, (sprite, curIndex), CCSprite* sprite, int* curIndex)
MISSING_IN_COCOS2D_PRIVATE PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCSpriteBatchNode, swap, (oldIndex, newIndex), int oldIndex, int newIndex)

PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCLabelBMFont, setString, (str, needUpdateLabel), const char* str, bool needUpdateLabel)
MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO(void, cocos2d::CCLabelBMFont, updateLabel)

MISSING_ON_MAC_COCOS2D PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCTextureAtlas, updateQuad, (quad, idk), ccV3F_C4B_T2F_Quad* quad, unsigned int idk)

PROFILER_HOOK_AUTO_ARGS(void, cocos2d::CCNode, update, (dt), float dt)
PROFILER_HOOK_AUTO(void, cocos2d::CCNode, visit)
PROFILER_HOOK_AUTO(void, cocos2d::CCNode, updateTransform)

// post frame? haven't actually seen this getting called so idk
MISSING_IN_COCOS2D_NOT_A_MEMBER PROFILER_HOOK_AUTO(void, cocos2d::CCApplication, updateVerticalSync)
MISSING_IN_COCOS2D_NOT_A_MEMBER PROFILER_HOOK_AUTO(void, cocos2d::CCApplication, updateControllerKeys)
