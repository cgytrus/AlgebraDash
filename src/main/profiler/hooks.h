#define PROFILER_HOOK_THIS_ARGS(ret, type, name, argsC, ...) PROFILER_HOOK(ret, type, name, argsC, (__VA_ARGS__))
#define PROFILER_HOOK_THIS(ret, type, name) PROFILER_HOOK(ret, type, name, (), ())
//#define PROFILER_HOOK_SAME_ARGS(address, ret, callConv, name, argsC, ...) PROFILER_HOOK(address, name, ret, callConv, callConv, (__VA_ARGS__), (__VA_ARGS__), argsC)
//#define PROFILER_HOOK_SAME(address, ret, callConv, name) PROFILER_HOOK(address, name, ret, callConv, callConv, (), (), ())

// haha rip dll size

// pre frame
PROFILER_HOOK_THIS(void, cocos2d::CCEGLView, pollEvents)
//PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x110dc0, void, __cdecl, cocos2d::CCEGLView::FUN_10110dc0, (idk), int idk)
//PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x111a80, void, __cdecl, cocos2d::CCEGLView::FUN_10111a80, (idk1, idk2, idk3, idk4, idk5), int idk1, int idk2, int idk3, int idk4, int idk5)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCDirector, setDeltaTime, (dt), float dt)

// actual frame
PROFILER_HOOK_THIS(void, cocos2d::CCDirector, drawScene)
PROFILER_HOOK_THIS(void, cocos2d::CCDirector, setNextScene)

// gd init
//PROFILER_HOOK_THIS_ARGS(GD + 0x16c4d0, bool, DrawGridLayer::init, (self, idk1, idk2), void* idk1, void* idk2) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x6f5d0, bool, EditLevelLayer::init, (self, level), gd::GJGameLevel* level) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x76310, bool, EditorUI::init, (self, level), gd::GJGameLevel* level) // missing in geode
PROFILER_HOOK_THIS_ARGS(bool, LevelEditorLayer, init, (level), GJGameLevel* level)
PROFILER_HOOK_THIS_ARGS(bool, LoadingLayer, init, (idk), char idk)
PROFILER_HOOK_THIS(bool, MenuLayer, init)
//PROFILER_HOOK_THIS(GD + 0x198b20, bool, ObjectToolbox::init) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x1e6da0, bool, PlayerObject::init, (self, idk1, idk2, idk3), int idk1, int idk2, CCLayer* idk3) // missing in geode // wtf nvm why is it 0x26a660 lmao
PROFILER_HOOK_THIS_ARGS(bool, PlayLayer, init, (level), GJGameLevel* level)

// gd update
//PROFILER_HOOK_THIS_ARGS(GD + 0x16cd80, void, gd::DrawGridLayer::update, (self, dt), float dt) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x23b20, void, gd::FMODAudioEngine::update, (self, dt), float dt) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0xce440, void, gd::GameManager::update, (self, dt), float dt) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x16a660, void, gd::LevelEditorLayer::update, (self, dt), float dt) // missing in geode
//PROFILER_HOOK_THIS_ARGS(GD + 0x1e8200, void, gd::PlayerObject::update, (self, dt), float dt) // missing in geode
PROFILER_HOOK_THIS_ARGS(void, PlayLayer, update, (dt), float dt)

//PROFILER_HOOK_THIS(GD + 0x11e1d0, void, gd::GJEffectManager::updateColorEffects) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11e7f0, void, gd::GJEffectManager::updatePulseEffects) // missing in geode on windows
PROFILER_HOOK_THIS_ARGS(void, GJEffectManager, updateOpacityEffects, (dt), float dt)
//PROFILER_HOOK_THIS(GD + 0x11f000, void, gd::GJEffectManager::updateSpawnTriggers) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11d030, void, gd::GJEffectManager::preCollisionCheck) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x11da30, void, gd::GJEffectManager::prepareMoveActions, (self, idk), bool idk) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x10ffa0, void, gd::PlayLayer::FUN_0050ffa0) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x1101a0, void, gd::PlayLayer::FUN_005101a0) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x1107e0, void, gd::PlayLayer::FUN_005107e0) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x110b10, void, gd::PlayLayer::FUN_00510b10) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x110e00, void, gd::PlayLayer::FUN_00510e00) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x10ef70, void, gd::PlayLayer::FUN_0050ef70) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x1e8b50, void, gd::PlayerObject::updateCheckpointTest) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11d0b0, void, gd::PlayLayer::FUN_0051d0b0) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x111b00, void, gd::PlayLayer::FUN_00511b00) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x2071d0, void, gd::PlayLayer::updateCamera) // missing in geode
//PROFILER_HOOK_THIS(GD + 0x206e10, void, gd::PlayLayer::loadDefaultColors) // missing in geode on windows
PROFILER_HOOK_THIS(void, PlayLayer, updateVisibility)
//PROFILER_HOOK_THIS(GD + 0x11c7c0, void, gd::GJEffectManager::calculateBaseActiveColors) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11ea50, void, gd::GJEffectManager::processPulseActions) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11c8a0, void, gd::GJEffectManager::processInheritedColors) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x11ebc0, void, gd::GJEffectManager::processCopyColorPulseActions) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x11ce20, int, gd::GJEffectManager::getColorSprite, (self, index), int index) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x11f420, void, gd::GJEffectManager::calculateLightBGColor, (self, idk), int idk) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x26a60, void, gd::GJEffectManager::FUN_00426a60, (self, idk1, idk2, idk3, idk4, idk5), void* idk1, float idk2, float idk3, float idk4, int idk5)
//PROFILER_HOOK_THIS(GD + 0xebda0, void, gd::idk::FUN_004ebda0)
//PROFILER_HOOK_THIS_ARGS(GD + 0xeb3f0, void, gd::GameObject::addColorSpriteToParent, (self, idk), bool idk) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x2531f0, void, gd::AnimatedGameObject::updateChildSpriteColor, (self, idk), int idk) // missing in geode on windows
PROFILER_HOOK_THIS_ARGS(void, GameObject, setObjectColor, (color), const ccColor3B& color)

PROFILER_HOOK_THIS(void, GameObject, updateOrientedBox)
//PROFILER_HOOK_THIS_ARGS(GD + 0x10fe10, void, gd::GJBaseGameLayer::reorderObjectSection, (self, obj), gd::GameObject* obj) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0x112470, void, std::vector<gd::GameObject*>::emplace, (self, elem), gd::GameObject* elem) // missing in geode?
//PROFILER_HOOK_THIS_ARGS(GD + 0x10ff30, void, gd::GJBaseGameLayer::removeObjectFromSection, (self, obj), gd::GameObject* obj) // missing in geode on windows

//PROFILER_HOOK_THIS(GD + 0xd1870, void, gd::GameObject::activateObject) // missing in geode on windows
//PROFILER_HOOK_THIS_ARGS(GD + 0xef0e0, void, gd::GameObject::activateObject, (self, idk), void* idk) // missing in geode on windows
PROFILER_HOOK_THIS_ARGS(void, GameObject, deactivateObject, (idk), bool idk)

PROFILER_HOOK_THIS_ARGS(bool, GameManager, getGameVariable, (key), const char* key)
PROFILER_HOOK_THIS_ARGS(void, GameManager, setGameVariable, (key, value), const char* key, bool value)

// gd draw
PROFILER_HOOK_THIS(void, DrawGridLayer, draw) // missing on mac in geode
//PROFILER_HOOK_THIS(GD + 0x18fbe0, void, gd::EditorUI::draw) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0x16b7c0, void, gd::LevelEditorLayer::draw) // missing in geode on windows

// loading
PROFILER_HOOK_THIS(void, LoadingLayer, loadAssets)
PROFILER_HOOK_THIS_ARGS(CCTexture2D*, cocos2d::CCTextureCache, addImage, (path, idk), const char* path, bool idk)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTextureCache, addImageAsync, (path, target, selector), const char* path, CCObject* target, SEL_CallFuncO selector)
//PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec7c0, void*, __cdecl, cocos2d::CCTextureCache::loadImage, (data), void* data)
//PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec9a0, void, __cdecl, cocos2d::CCTextureCache::loadImageData, (pAsyncStruct), void* pAsyncStruct)
//PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithDictionary, (dict, texture), CCDictionary* dict, CCTexture2D* texture) // private
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath, texturePath), const char* plistPath, const char* texturePath)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath), const char* plistPath)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteFrameCache, addSpriteFramesWithFile, (plistPath, texture), const char* plistPath, CCTexture2D* texture)
PROFILER_HOOK_THIS_ARGS(static CCLabelBMFont*, cocos2d::CCLabelBMFont, create, (idk1, idk2, idk3, idk4, idk5), const char* idk1, const char* idk2, float idk3, CCTextAlignment idk4, CCPoint idk5)
//PROFILER_HOOK_THIS(GD + 0x25520, void, gd::GameSoundManager::asynchronousSetup) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0xc9420, void, gd::GameManager::FUN_0049420) // missing in geode on windows
//PROFILER_HOOK_THIS(GD + 0xa8cb0, void, gd::GameLevelManager::FUN_004a8cb0) // missing in geode on windows
PROFILER_HOOK_THIS_ARGS(bool, cocos2d::CCTexture2D, initWithImage, (image), CCImage* image)
PROFILER_HOOK_THIS_ARGS(static CCBMFontConfiguration*, cocos2d::CCBMFontConfiguration, create, (fntFile), const char* fntFile)
//PROFILER_HOOK_THIS_ARGS(void*, cocos2d::CCBMFontConfiguration, parseConfigFile, (fntFile), const char* fntFile) // private
PROFILER_HOOK_THIS_ARGS(bool, cocos2d::CCSpriteBatchNode, initWithTexture, (texture, capacity), CCTexture2D* texture, unsigned int capacity)
PROFILER_HOOK_THIS_ARGS(bool, cocos2d::CCSprite, initWithTexture, (texture, rect, rotated), CCTexture2D* texture, const CCRect& rect, bool rotated)
PROFILER_HOOK_THIS_ARGS(bool, cocos2d::CCTextureAtlas, initWithTexture, (texture, capacity), CCTexture2D* texture, unsigned int capacity)
//PROFILER_HOOK_THIS(void, cocos2d::CCTextureAtlas, setupIndices) // private
//PROFILER_HOOK_THIS(void, cocos2d::CCTextureAtlas, mapBuffers) // private

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCScheduler, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCAccelerometer, update, (idk1, idk2, idk3, idk4), double idk1, double idk2, double idk3, double idk4)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTimer, update, (dt), float dt)

// actions
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCActionManager, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTargetedAction, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCActionEase, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCActionTween, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCAnimate, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCCallFunc, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSequence, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCBezierBy, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCBlink, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCCardinalSplineTo, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFadeIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFadeOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFadeOutTRTiles, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFadeTo, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFlipX, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCFlipY, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCJumpBy, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCMoveBy, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCPlace, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCRemoveSelf, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCRepeat, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCReverseTime, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCRotateBy, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCRotateTo, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCScaleTo, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpawn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTintBy, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTintTo, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCToggleVisibility, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCHide, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCShow, update, (dt), float dt)

// progress
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCProgressFromTo, update, (dt), float dt)
PROFILER_HOOK_THIS(void, cocos2d::CCProgressTimer, updateBar)
PROFILER_HOOK_THIS(void, cocos2d::CCProgressTimer, updateColor)
PROFILER_HOOK_THIS(void, cocos2d::CCProgressTimer, updateProgress)
PROFILER_HOOK_THIS(void, cocos2d::CCProgressTimer, updateRadial)

// easings
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBackIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBackInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBackOut, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBounceIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBounceInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseBounceOut, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseElasticIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseElasticInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseElasticOut, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseExponentialIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseExponentialInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseExponentialOut, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseOut, update, (dt), float dt)

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseSineIn, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseSineInOut, update, (dt), float dt)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCEaseSineOut, update, (dt), float dt)

// for some reason it keeps erroring "custom modify not implemented" about random cocos hooks
// so i'm just gonna leave it at this for now and fix everything later

// particles
//PROFILER_HOOK_THIS(void, cocos2d::CCParticleBatchNode, draw) // custom modify not implemented??????????
//PROFILER_HOOK_THIS(void, cocos2d::CCParticleBatchNode, visit) // custom modify not implemented??????????
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCParticleSystem, update, (dt), float dt)
PROFILER_HOOK_THIS(void, cocos2d::CCParticleSystem, updateWithNoTime)
PROFILER_HOOK_THIS(void, cocos2d::CCParticleSystemQuad, draw)

// sprites
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSprite, setOpacity, (opacity), GLubyte opacity)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSprite, setColor, (color3), const ccColor3B& color3)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSprite, setOpacityModifyRGB, (modify), bool modify)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSprite, updateDisplayedColor, (parentColor), const ccColor3B& parentColor)
PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSprite, updateDisplayedOpacity, (opacity), GLubyte opacity)
PROFILER_HOOK_THIS(void, cocos2d::CCSprite, updateColor)
PROFILER_HOOK_THIS(void, cocos2d::CCSprite, updateTransform)
PROFILER_HOOK_THIS(void, cocos2d::CCSprite, draw)
PROFILER_HOOK_THIS(CCSpriteFrame*, cocos2d::CCSprite, displayFrame)
PROFILER_HOOK_THIS(void, cocos2d::CCSpriteBatchNode, draw)
PROFILER_HOOK_THIS(void, cocos2d::CCSpriteBatchNode, sortAllChildren)
//PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteBatchNode, updateAtlastIndex, (sprite, curIndex), CCSprite* sprite, int* curIndex) // is not a member????
//PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCSpriteBatchNode, swap, (oldIndex, newIndex), int oldIndex, int newIndex) // private

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCLabelBMFont, setString, (str, needUpdateLabel), const char* str, bool needUpdateLabel)
PROFILER_HOOK_THIS(void, cocos2d::CCLabelBMFont, updateLabel)

//PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCTextureAtlas, updateQuad, (quad, idk), ccV3F_C4B_T2F_Quad* quad, unsigned int idk) // custom modify not implemented??????????

PROFILER_HOOK_THIS_ARGS(void, cocos2d::CCNode, update, (dt), float dt)
PROFILER_HOOK_THIS(void, cocos2d::CCNode, visit)
PROFILER_HOOK_THIS(void, cocos2d::CCNode, updateTransform)

// post frame? haven't actually seen this getting called so idk
//PROFILER_HOOK_THIS(void, cocos2d::CCApplication, updateVerticalSync) // is not a member????
//PROFILER_HOOK_THIS(void, cocos2d::CCApplication, updateControllerKeys) // is not a member????
