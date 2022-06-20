#include "../../includes.h"

#define PROFILER_HOOK_THIS_ARGS(address, ret, name, argsC, ...) PROFILER_HOOK(address, name, ret, __thiscall, __fastcall, (void*, __VA_ARGS__), (void* self, void*, __VA_ARGS__), argsC)
#define PROFILER_HOOK_THIS(address, ret, name) PROFILER_HOOK(address, name, ret, __thiscall, __fastcall, (void*), (void* self), (self))
#define PROFILER_HOOK_SAME_ARGS(address, ret, callConv, name, argsC, ...) PROFILER_HOOK(address, name, ret, callConv, callConv, (__VA_ARGS__), (__VA_ARGS__), argsC)
#define PROFILER_HOOK_SAME(address, ret, callConv, name) PROFILER_HOOK(address, name, ret, callConv, callConv, (), (), ())

// haha rip dll size

// pre frame
PROFILER_HOOK_THIS(COCOS2DSYM("?pollEvents@CCEGLView@cocos2d@@QAEXXZ"), void, cocos2d::CCEGLView::pollEvents)
PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x110dc0, void, __cdecl, cocos2d::CCEGLView::FUN_10110dc0, (idk), int idk)
PROFILER_HOOK_SAME_ARGS(COCOS2D + 0x111a80, void, __cdecl, cocos2d::CCEGLView::FUN_10111a80, (idk1, idk2, idk3, idk4, idk5), int idk1, int idk2, int idk3, int idk4, int idk5)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?setDeltaTime@CCDirector@cocos2d@@QAEXM@Z"), void, cocos2d::CCDirector::setDeltaTime, (self, dt), float dt)

// actual frame
PROFILER_HOOK_THIS(COCOS2DSYM("?drawScene@CCDirector@cocos2d@@QAEXXZ"), void, cocos2d::CCDirector::drawScene)
PROFILER_HOOK_THIS(COCOS2DSYM("?setNextScene@CCDirector@cocos2d@@IAEXXZ"), void, cocos2d::CCDirector::setNextScene)

// gd init
PROFILER_HOOK_THIS_ARGS(GD + 0x16c4d0, bool, gd::DrawGridLayer::init, (self, grid, editor), CCNode* grid, gd::LevelEditorLayer* editor)
PROFILER_HOOK_THIS_ARGS(GD + 0x6f5d0, bool, gd::EditLevelLayer::init, (self, level), gd::GJGameLevel* level)
PROFILER_HOOK_THIS_ARGS(GD + 0x76310, bool, gd::EditorUI::init, (self, level), gd::LevelEditorLayer* level)
PROFILER_HOOK_THIS_ARGS(GD + 0x15ee00, bool, gd::LevelEditorLayer::init, (self, level), gd::GJGameLevel* level)
PROFILER_HOOK_THIS_ARGS(GD + 0x18c080, bool, gd::LoadingLayer::init, (self, fromReload), bool fromReload)
PROFILER_HOOK_THIS(GD + 0x1907b0, bool, gd::MenuLayer::init)
PROFILER_HOOK_THIS(GD + 0x198b20, bool, gd::ObjectToolbox::init)
PROFILER_HOOK_THIS_ARGS(GD + 0x1e6da0, bool, gd::PlayerObject::init, (self, idk1, idk2, idk3), int idk1, int idk2, CCLayer* idk3)
PROFILER_HOOK_THIS_ARGS(GD + 0x1fb780, bool, gd::PlayLayer::init, (self, level), gd::GJGameLevel* level)

// gd update
PROFILER_HOOK_THIS_ARGS(GD + 0x16cd80, void, gd::DrawGridLayer::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x23b20, void, gd::FMODAudioEngine::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0xce440, void, gd::GameManager::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x16a660, void, gd::LevelEditorLayer::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x1e8200, void, gd::PlayerObject::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x2029c0, void, gd::PlayLayer::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(GD + 0x11e1d0, void, gd::GJEffectManager::updateColorEffects, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x11e7f0, void, gd::GJEffectManager::updatePulseEffect, (self, dt), float dts)
PROFILER_HOOK_THIS_ARGS(GD + 0x11e490, void, gd::GJEffectManager::updateOpacityEffects, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(GD + 0x11f000, void, gd::GJEffectManager::updateSpawnTriggers, (self, dt), float dt)
PROFILER_HOOK_THIS(GD + 0x11d030, void, gd::GJEffectManager::preCollisionCheck)
PROFILER_HOOK_THIS_ARGS(GD + 0x11da30, void, gd::GJEffectManager::prepareMoveActions, (self, idk1, idk2), float idk1, bool idk2)
PROFILER_HOOK_THIS_ARGS(GD + 0x10ffa0, void, gd::PlayLayer::processMoveActionsStep, (self, dt), float dt)
PROFILER_HOOK_THIS(GD + 0x1101a0, void, gd::PlayLayer::processRotationActions)
PROFILER_HOOK_THIS(GD + 0x1107e0, void, gd::PlayLayer::processMoveActions)
PROFILER_HOOK_THIS_ARGS(GD + 0x110b10, void, gd::PlayLayer::processPlayerFollowActions, (self, dt), float dt)
PROFILER_HOOK_THIS(GD + 0x110e00, void, gd::PlayLayer::processFollowActions)
PROFILER_HOOK_THIS(GD + 0x10ef70, void, gd::PlayLayer::updateCollisionBlocks)
PROFILER_HOOK_THIS(GD + 0x1e8b50, void, gd::PlayerObject::updateCheckpointTest)
PROFILER_HOOK_THIS(GD + 0x11d0b0, void, gd::PlayLayer::postCollisionCheck)
PROFILER_HOOK_THIS(GD + 0x111b00, void, gd::PlayLayer::updateQueuedLabels)
PROFILER_HOOK_THIS_ARGS(GD + 0x2071d0, void, gd::PlayLayer::updateCamera, (self, dt), float dt)
PROFILER_HOOK_THIS(GD + 0x205460, void, gd::PlayLayer::updateVisibility)
PROFILER_HOOK_THIS(GD + 0x11c7c0, void, gd::GJEffectManager::calculateBaseActiveColors)
PROFILER_HOOK_THIS(GD + 0x11ea50, void, gd::GJEffectManager::processPulseActions)
PROFILER_HOOK_THIS(GD + 0x11c8a0, void, gd::GJEffectManager::processInheritedColors)
PROFILER_HOOK_THIS(GD + 0x11ebc0, void, gd::GJEffectManager::processCopyColorPulseActions)
PROFILER_HOOK_THIS_ARGS(GD + 0x11ce20, const ccColor3B&, gd::GJEffectManager::getColorSprite, (self, index), int index)
PROFILER_HOOK_THIS_ARGS(GD + 0x11f420, void, gd::GJEffectManager::calculateLightBGColor, (self, color3), ccColor3B color3)
//PROFILER_HOOK_THIS_ARGS(GD + 0x26a60, void, gd::GJEffectManager::FUN_00426a60, (self, idk1, idk2, idk3, idk4, idk5), void* idk1, float idk2, float idk3, float idk4, int idk5)
//PROFILER_HOOK_THIS(GD + 0xebda0, void, gd::idk::FUN_004ebda0)
PROFILER_HOOK_THIS_ARGS(GD + 0xeb3f0, void, gd::GameObject::addColorSpriteToParent, (self, idk), bool idk)
PROFILER_HOOK_THIS_ARGS(GD + 0x2531f0, void, gd::AnimatedGameObject::updateChildSpriteColor, (self, color3), ccColor3B color3)
PROFILER_HOOK_THIS_ARGS(GD + 0xee620, void, gd::GameObject::setObjectColor, (self, color3), const ccColor3B& color3)

PROFILER_HOOK_THIS(GD + 0xef1c0, void, gd::GameObject::updateOrientedBox)
PROFILER_HOOK_THIS_ARGS(GD + 0x10fe10, void, gd::GJBaseGameLayer::reorderObjectSection, (self, obj), gd::GameObject* obj)
PROFILER_HOOK_THIS_ARGS(GD + 0x112470, void, std::vector<gd::GameObject*>::emplace, (self, elem), gd::GameObject* elem)
PROFILER_HOOK_THIS_ARGS(GD + 0x10ff30, void, gd::GJBaseGameLayer::removeObjectFromSection, (self, obj), gd::GameObject* obj)

PROFILER_HOOK_THIS(GD + 0xd1870, void, gd::GameObject::activateObject)
PROFILER_HOOK_THIS_ARGS(GD + 0xef0e0, void, gd::GameObject::activateObject, (self, player), PlayerObject* player)
PROFILER_HOOK_THIS_ARGS(GD + 0xd19b0, void, gd::GameObject::deactivateObject, (self, idk), bool idk)

PROFILER_HOOK_THIS_ARGS(GD + 0xc9d30, bool, gd::GameManager::getGameVariable, (self, key), const char* key)
PROFILER_HOOK_THIS_ARGS(GD + 0xc9b50, void, gd::GameManager::setGameVariable, (self, key, value), const char* key, bool value)

// gd draw
PROFILER_HOOK_THIS(GD + 0x16ce90, void, gd::DrawGridLayer::draw)
PROFILER_HOOK_THIS(GD + 0x18fbe0, void, gd::EditorUI::draw)
PROFILER_HOOK_THIS(GD + 0x16b7c0, void, gd::LevelEditorLayer::draw)

// loading
PROFILER_HOOK_THIS(GD + 0x18c8e0, void, gd::LoadingLayer::loadAssets)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addImage@CCTextureCache@cocos2d@@QAEPAVCCTexture2D@2@PBD_N@Z"), CCTexture2D*, cocos2d::CCTextureCache::addImage, (self, path, idk), const char* path, bool idk)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addImageAsync@CCTextureCache@cocos2d@@QAEXPBDPAVCCObject@2@P832@AEX1@Z@Z"), CCTexture2D*, cocos2d::CCTextureCache::addImageAsync, (self, path, target, selector), const char* path, CCObject* target, SEL_CallFuncO selector)
PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec7c0, void*, __cdecl, cocos2d::CCTextureCache::loadImage, (data), void* data)
PROFILER_HOOK_SAME_ARGS(COCOS2D + 0xec9a0, void, __cdecl, cocos2d::CCTextureCache::loadImageData, (pAsyncStruct), void* pAsyncStruct)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addSpriteFramesWithDictionary@CCSpriteFrameCache@cocos2d@@AAEXPAVCCDictionary@2@PAVCCTexture2D@2@@Z"), void, cocos2d::CCSpriteFrameCache::addSpriteFramesWithDictionary, (self, dict, texture), CCDictionary* dict, CCTexture2D* texture)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBD0@Z"), void, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, (self, plistPath, texturePath), const char* plistPath, const char* texturePath)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBD@Z"), void, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, (self, plistPath), const char* plistPath)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBDPAVCCTexture2D@2@@Z"), void, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, (self, plistPath, texture), const char* plistPath, CCTexture2D* texture)
PROFILER_HOOK_SAME_ARGS(COCOS2DSYM("?create@CCLabelBMFont@cocos2d@@SAPAV12@PBD0MW4CCTextAlignment@2@VCCPoint@2@@Z"), CCLabelBMFont*, __cdecl, cocos2d::CCLabelBMFont::create, (idk1, idk2, idk3, idk4, idk5), const char* idk1, const char* idk2, float idk3, CCTextAlignment idk4, CCPoint idk5)
PROFILER_HOOK_THIS(GD + 0x25520, void, gd::GameSoundManager::asynchronousSetup)
PROFILER_HOOK_THIS(GD + 0xc9420, void, gd::GameManager::FUN_0049420)
PROFILER_HOOK_THIS(GD + 0xa8cb0, void, gd::GameLevelManager::FUN_004a8cb0)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?initWithImage@CCTexture2D@cocos2d@@QAE_NPAVCCImage@2@@Z"), void, cocos2d::CCTexture2D::initWithImage, (self, image), CCImage* image)
PROFILER_HOOK_SAME_ARGS(COCOS2DSYM("?create@CCBMFontConfiguration@cocos2d@@SAPAV12@PBD@Z"), CCBMFontConfiguration*, __cdecl, cocos2d::CCBMFontConfiguration::create, (fntFile), const char* fntFile)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?parseConfigFile@CCBMFontConfiguration@cocos2d@@AAEPAV?$set@IU?$less@I@std@@V?$allocator@I@2@@std@@PBD@Z"), void*, cocos2d::CCBMFontConfiguration::parseConfigFile, (self, fntFile), const char* fntFile)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?initWithTexture@CCSpriteBatchNode@cocos2d@@QAE_NPAVCCTexture2D@2@I@Z"), bool, cocos2d::CCSpriteBatchNode::initWithTexture, (self, texture, capacity), CCTexture2D* texture, unsigned int capacity)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?initWithTexture@CCSprite@cocos2d@@UAE_NPAVCCTexture2D@2@ABVCCRect@2@_N@Z"), bool, cocos2d::CCSprite::initWithTexture, (self, texture, rect, rotated), CCTexture2D* texture, CCRect* rect, bool rotated)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?initWithTexture@CCTextureAtlas@cocos2d@@QAE_NPAVCCTexture2D@2@I@Z"), bool, cocos2d::CCTextureAtlas::initWithTexture, (self, texture, capacity), CCTexture2D* texture, unsigned int capacity)
PROFILER_HOOK_THIS(COCOS2DSYM("?setupIndices@CCTextureAtlas@cocos2d@@AAEXXZ"), void, cocos2d::CCTextureAtlas::setupIndices)
PROFILER_HOOK_THIS(COCOS2DSYM("?mapBuffers@CCTextureAtlas@cocos2d@@AAEXXZ"), void, cocos2d::CCTextureAtlas::mapBuffers)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCScheduler@cocos2d@@UAEXM@Z"), void, cocos2d::CCScheduler::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCAccelerometer@cocos2d@@QAEXNNNN@Z"), void, cocos2d::CCAccelerometer::update, (self, idk1, idk2, idk3, idk4), double idk1, double idk2, double idk3, double idk4)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCTimer@cocos2d@@UAEXM@Z"), void, cocos2d::CCTimer::update, (self, dt), float dt)

// actions
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCActionManager@cocos2d@@MAEXM@Z"), void, cocos2d::CCActionManager::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCTargetedAction@cocos2d@@UAEXM@Z"), void, cocos2d::CCTargetedAction::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCActionEase@cocos2d@@UAEXM@Z"), void, cocos2d::CCActionEase::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCActionTween@cocos2d@@UAEXM@Z"), void, cocos2d::CCActionTween::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCAnimate@cocos2d@@UAEXM@Z"), void, cocos2d::CCAnimate::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCCallFunc@cocos2d@@UAEXM@Z"), void, cocos2d::CCCallFunc::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCSequence@cocos2d@@UAEXM@Z"), void, cocos2d::CCSequence::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCBezierBy@cocos2d@@UAEXM@Z"), void, cocos2d::CCBezierBy::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCBlink@cocos2d@@UAEXM@Z"), void, cocos2d::CCBlink::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCCardinalSplineTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCCardinalSplineTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFadeIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCFadeIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFadeOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCFadeOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFadeOutTRTiles@cocos2d@@UAEXM@Z"), void, cocos2d::CCFadeOutTRTiles::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFadeTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCFadeTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFlipX@cocos2d@@UAEXM@Z"), void, cocos2d::CCFlipX::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCFlipY@cocos2d@@UAEXM@Z"), void, cocos2d::CCFlipY::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCJumpBy@cocos2d@@UAEXM@Z"), void, cocos2d::CCJumpBy::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCMoveBy@cocos2d@@UAEXM@Z"), void, cocos2d::CCMoveBy::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCPlace@cocos2d@@UAEXM@Z"), void, cocos2d::CCPlace::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCRemoveSelf@cocos2d@@UAEXM@Z"), void, cocos2d::CCRemoveSelf::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCRepeat@cocos2d@@UAEXM@Z"), void, cocos2d::CCRepeat::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCReverseTime@cocos2d@@UAEXM@Z"), void, cocos2d::CCReverseTime::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCRotateBy@cocos2d@@UAEXM@Z"), void, cocos2d::CCRotateBy::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCRotateTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCRotateTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCScaleTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCScaleTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCSpawn@cocos2d@@UAEXM@Z"), void, cocos2d::CCSpawn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCTintBy@cocos2d@@UAEXM@Z"), void, cocos2d::CCTintBy::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCTintTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCTintTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCToggleVisibility@cocos2d@@UAEXM@Z"), void, cocos2d::CCToggleVisibility::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCHide@cocos2d@@UAEXM@Z"), void, cocos2d::CCHide::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCShow@cocos2d@@UAEXM@Z"), void, cocos2d::CCShow::update, (self, dt), float dt)

// progress
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCProgressFromTo@cocos2d@@UAEXM@Z"), void, cocos2d::CCProgressFromTo::update, (self, dt), float dt)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateBar@CCProgressTimer@cocos2d@@IAEXXZ"), void, cocos2d::CCProgressTimer::updateBar)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateColor@CCProgressTimer@cocos2d@@QAEXXZ"), void, cocos2d::CCProgressTimer::updateColor)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateProgress@CCProgressTimer@cocos2d@@IAEXXZ"), void, cocos2d::CCProgressTimer::updateProgress)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateRadial@CCProgressTimer@cocos2d@@IAEXXZ"), void, cocos2d::CCProgressTimer::updateRadial)

// easings
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBackIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBackIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBackInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBackInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBackOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBackOut::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBounceIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBounceIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBounceInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBounceInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseBounceOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseBounceOut::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseElasticIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseElasticIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseElasticInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseElasticInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseElasticOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseElasticOut::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseExponentialIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseExponentialIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseExponentialInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseExponentialInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseExponentialOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseExponentialOut::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseOut::update, (self, dt), float dt)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseSineIn@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseSineIn::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseSineInOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseSineInOut::update, (self, dt), float dt)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCEaseSineOut@cocos2d@@UAEXM@Z"), void, cocos2d::CCEaseSineOut::update, (self, dt), float dt)

// particles
PROFILER_HOOK_THIS(COCOS2DSYM("?draw@CCParticleBatchNode@cocos2d@@UAEXXZ"), void, cocos2d::CCParticleBatchNode::draw)
PROFILER_HOOK_THIS(COCOS2DSYM("?visit@CCParticleBatchNode@cocos2d@@UAEXXZ"), void, cocos2d::CCParticleBatchNode::visit)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCParticleSystem@cocos2d@@UAEXM@Z"), void, cocos2d::CCParticleSystem::update, (self, dt), float dt)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateWithNoTime@CCParticleSystem@cocos2d@@UAEXXZ"), void, cocos2d::CCParticleSystem::updateWithNoTime)
PROFILER_HOOK_THIS(COCOS2DSYM("?draw@CCParticleSystemQuad@cocos2d@@UAEXXZ"), void, cocos2d::CCParticleSystemQuad::draw)

// sprites
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?setOpacity@CCSprite@cocos2d@@UAEXE@Z"), void, cocos2d::CCSprite::setOpacity, (self, opacity), GLubyte opacity)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?setColor@CCSprite@cocos2d@@UAEXABU_ccColor3B@2@@Z"), void, cocos2d::CCSprite::setColor, (self, color3), const ccColor3B& color3)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?setOpacityModifyRGB@CCSprite@cocos2d@@UAEX_N@Z"), void, cocos2d::CCSprite::setOpacityModifyRGB, (self, modify), bool modify)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?updateDisplayedColor@CCSprite@cocos2d@@UAEXABU_ccColor3B@2@@Z"), void, cocos2d::CCSprite::updateDisplayedColor, (self, parentColor), const ccColor3B& parentColor)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?updateDisplayedOpacity@CCSprite@cocos2d@@UAEXE@Z"), void, cocos2d::CCSprite::updateDisplayedOpacity, (self, opacity), GLubyte opacity)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateColor@CCSprite@cocos2d@@IAEXXZ"), void, cocos2d::CCSprite::updateColor)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateTransform@CCSprite@cocos2d@@UAEXXZ"), void, cocos2d::CCSprite::updateTransform)
PROFILER_HOOK_THIS(COCOS2DSYM("?draw@CCSprite@cocos2d@@UAEXXZ"), void, cocos2d::CCSprite::draw)
PROFILER_HOOK_THIS(COCOS2DSYM("?displayFrame@CCSprite@cocos2d@@UAEPAVCCSpriteFrame@2@XZ"), CCSpriteFrame*, cocos2d::CCSprite::displayFrame)
PROFILER_HOOK_THIS(COCOS2DSYM("?draw@CCSpriteBatchNode@cocos2d@@UAEXXZ"), void, cocos2d::CCSpriteBatchNode::draw)
PROFILER_HOOK_THIS(COCOS2DSYM("?sortAllChildren@CCSpriteBatchNode@cocos2d@@UAEXXZ"), void, cocos2d::CCSpriteBatchNode::sortAllChildren)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?updateAtlasIndex@CCSpriteBatchNode@cocos2d@@AAEXPAVCCSprite@2@PAH@Z"), void, cocos2d::CCSpriteBatchNode::updateAtlastIndex, (self, sprite, curIndex), CCSprite* sprite, int* curIndex)
PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?swap@CCSpriteBatchNode@cocos2d@@AAEXHH@Z"), void, cocos2d::CCSpriteBatchNode::swap, (self, oldIndex, newIndex), int oldIndex, int newIndex)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?setString@CCLabelBMFont@cocos2d@@UAEXPBD_N@Z"), void, cocos2d::CCLabelBMFont::setString, (self, str, needUpdateLabel), const char* str, bool needUpdateLabel)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateLabel@CCLabelBMFont@cocos2d@@UAEXXZ"), void, cocos2d::CCLabelBMFont::updateLabel)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?updateQuad@CCTextureAtlas@cocos2d@@QAEXPAU_ccV3F_C4B_T2F_Quad@2@I@Z"), void, cocos2d::CCTextureAtlas::updateQuad, (self, quad, idk), ccV3F_C4B_T2F_Quad* quad, unsigned int idk)

PROFILER_HOOK_THIS_ARGS(COCOS2DSYM("?update@CCNode@cocos2d@@UAEXM@Z"), void, cocos2d::CCNode::update, (self, dt), float dt)
PROFILER_HOOK_THIS(COCOS2DSYM("?visit@CCNode@cocos2d@@UAEXXZ"), void, cocos2d::CCNode::visit)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateTransform@CCNode@cocos2d@@UAEXXZ"), void, cocos2d::CCNode::updateTransform)

// post frame? haven't actually seen this getting called so idk
PROFILER_HOOK_THIS(COCOS2DSYM("?updateVerticalSync@CCApplication@cocos2d@@QAEXXZ"), void, cocos2d::CCApplication::updateVerticalSync)
PROFILER_HOOK_THIS(COCOS2DSYM("?updateControllerKeys@CCApplication@cocos2d@@QAEXXZ"), void, cocos2d::CCApplication::updateControllerKeys)
