#include "../../includes.h"

// haha rip dll size

// pre frame
PROFILER_HOOK(COCOS2DSYM("?pollEvents@CCEGLView@cocos2d@@QAEXXZ"), void, thiscall, cocos2d::CCEGLView::pollEvents, void*)
PROFILER_HOOK(COCOS2D + 0x110dc0, void, c_decl, cocos2d::CCEGLView::FUN_10110dc0, int)
PROFILER_HOOK(COCOS2D + 0x111a80, void, c_decl, cocos2d::CCEGLView::FUN_10111a80, int, int, int, int, int)
PROFILER_HOOK(COCOS2DSYM("?setDeltaTime@CCDirector@cocos2d@@QAEXM@Z"), void, membercall, cocos2d::CCDirector::setDeltaTime, void*, float)

// actual frame
PROFILER_HOOK(COCOS2DSYM("?drawScene@CCDirector@cocos2d@@QAEXXZ"), void, thiscall, cocos2d::CCDirector::drawScene, void*)
PROFILER_HOOK(COCOS2DSYM("?setNextScene@CCDirector@cocos2d@@IAEXXZ"), void, thiscall, cocos2d::CCDirector::setNextScene, void*)

// gd init
PROFILER_HOOK(GD + 0x16c4d0, bool, thiscall, gd::DrawGridLayer::init, CCNode*, void*, gd::LevelEditorLayer*)
PROFILER_HOOK(GD + 0x6f5d0, bool, thiscall, gd::EditLevelLayer::init, void*, gd::GJGameLevel*)
PROFILER_HOOK(GD + 0x76310, bool, thiscall, gd::EditorUI::init, void*, gd::LevelEditorLayer*)
PROFILER_HOOK(GD + 0x15ee00, bool, thiscall, gd::LevelEditorLayer::init, void*, gd::GJGameLevel*)
PROFILER_HOOK(GD + 0x18c080, bool, thiscall, gd::LoadingLayer::init, void*, bool)
PROFILER_HOOK(GD + 0x1907b0, bool, thiscall, gd::MenuLayer::init, void*)
PROFILER_HOOK(GD + 0x198b20, bool, thiscall, gd::ObjectToolbox::init, void*)
PROFILER_HOOK(GD + 0x1e6da0, bool, thiscall, gd::PlayerObject::init, void*, int, int, CCLayer*)
PROFILER_HOOK(GD + 0x1fb780, bool, thiscall, gd::PlayLayer::init, void*, gd::GJGameLevel*)

// gd update
PROFILER_HOOK(GD + 0x16cd80, void, membercall, gd::DrawGridLayer::update, void*, float)
PROFILER_HOOK(GD + 0x23b20, void, membercall, gd::FMODAudioEngine::update, void*, float)
PROFILER_HOOK(GD + 0xce440, void, membercall, gd::GameManager::update, void*, float)
PROFILER_HOOK(GD + 0x16a660, void, membercall, gd::LevelEditorLayer::update, void*, float)
PROFILER_HOOK(GD + 0x1e8200, void, membercall, gd::PlayerObject::update, void*, float)
PROFILER_HOOK(GD + 0x2029c0, void, membercall, gd::PlayLayer::update, void*, float)

  // for some reason float args in these hooks make the game crash, works fine with them commented out
  PROFILER_HOOK(GD + 0x11e1d0, void, membercall, gd::GJEffectManager::updateColorEffects, void* /*, float*/)
  PROFILER_HOOK(GD + 0x11e7f0, void, membercall, gd::GJEffectManager::updatePulseEffects, void* /*, float*/)
  PROFILER_HOOK(GD + 0x11e490, void, membercall, gd::GJEffectManager::updateOpacityEffects, void* /*, float*/)
  PROFILER_HOOK(GD + 0x11f000, void, membercall, gd::GJEffectManager::updateSpawnTriggers, void* /*, float*/)
PROFILER_HOOK(GD + 0x11d030, void, thiscall, gd::GJEffectManager::preCollisionCheck, void*)
  PROFILER_HOOK(GD + 0x11da30, void, membercall, gd::GJEffectManager::prepareMoveActions, void*, /*float,*/ bool)
  PROFILER_HOOK(GD + 0x10ffa0, void, membercall, gd::GJBaseGameLayer::processMoveActionsStep, void* /*, float*/)
PROFILER_HOOK(GD + 0x1101a0, void, thiscall, gd::GJBaseGameLayer::processRotationActions, void*)
PROFILER_HOOK(GD + 0x1107e0, void, thiscall, gd::GJBaseGameLayer::processMoveActions, void*)
  PROFILER_HOOK(GD + 0x110b10, void, membercall, gd::GJBaseGameLayer::processPlayerFollowActions, void* /*, float*/)
PROFILER_HOOK(GD + 0x110e00, void, thiscall, gd::PlayLayer::processFollowActions, void*)
PROFILER_HOOK(GD + 0x10ef70, void, thiscall, gd::PlayLayer::updateCollisionBlocks, void*)
PROFILER_HOOK(GD + 0x1e8b50, void, thiscall, gd::PlayerObject::updateCheckpointTest, void*)
PROFILER_HOOK(GD + 0x11d0b0, void, thiscall, gd::PlayLayer::postCollisionCheck, void*)
PROFILER_HOOK(GD + 0x111b00, void, thiscall, gd::PlayLayer::updateQueuedLabels, void*)
  PROFILER_HOOK(GD + 0x2071d0, void, membercall, gd::PlayLayer::updateCamera, void* /*, float*/)
PROFILER_HOOK(GD + 0x205460, void, thiscall, gd::PlayLayer::updateVisibility, void*)
PROFILER_HOOK(GD + 0x203a90, void, thiscall, gd::PlayLayer::updateLevelColors, void*)
PROFILER_HOOK(GD + 0x208020, void, thiscall, gd::PlayLayer::updateProgressbar, void*)
PROFILER_HOOK(GD + 0x20aab0, void, thiscall, gd::PlayLayer::updateEffectPositions, void*)
PROFILER_HOOK(GD + 0x11c7c0, void, thiscall, gd::GJEffectManager::calculateBaseActiveColors, void*)
PROFILER_HOOK(GD + 0x11ea50, void, thiscall, gd::GJEffectManager::processPulseActions, void*)
PROFILER_HOOK(GD + 0x11c8a0, void, thiscall, gd::GJEffectManager::processInheritedColors, void*)
PROFILER_HOOK(GD + 0x11ebc0, void, thiscall, gd::GJEffectManager::processCopyColorPulseActions, void*)
PROFILER_HOOK(GD + 0x11ce20, const ccColor3B&, thiscall, gd::GJEffectManager::getColorSprite, void*, int)
PROFILER_HOOK(GD + 0x11f420, void, thiscall, gd::GJEffectManager::calculateLightBGColor, void*, ccColor3B)
//PROFILER_HOOK_THIS_ARGS(GD + 0x26a60, void, gd::GJEffectManager::FUN_00426a60, (self, idk1, idk2, idk3, idk4, idk5), void* idk1, float idk2, float idk3, float idk4, int idk5)
//PROFILER_HOOK_THIS_ARGS(GD + 0xebda0, void, gd::idk::FUN_004ebda0)
PROFILER_HOOK(GD + 0xeb3f0, void, thiscall, gd::GameObject::addColorSpriteToParent, void*, bool)
PROFILER_HOOK(GD + 0x2531f0, void, thiscall, gd::AnimatedGameObject::updateChildSpriteColor, void*, ccColor3B)
PROFILER_HOOK(GD + 0xee620, void, thiscall, gd::GameObject::setObjectColor, void*, const ccColor3B&)

PROFILER_HOOK(GD + 0xef1c0, void, thiscall, gd::GameObject::updateOrientedBox, void*)
PROFILER_HOOK(GD + 0x10fe10, void, thiscall, gd::GJBaseGameLayer::reorderObjectSection, void*, gd::GameObject*)
PROFILER_HOOK(GD + 0x112470, void, thiscall, std::vector<gd::GameObject*>::emplace, void*, gd::GameObject*)
PROFILER_HOOK(GD + 0x10ff30, void, thiscall, gd::GJBaseGameLayer::removeObjectFromSection, void*, gd::GameObject*)

PROFILER_HOOK(GD + 0xd1870, void, thiscall, gd::GameObject::activateObject, void*)
PROFILER_HOOK(GD + 0xef0e0, void, thiscall, gd::GameObject::activateObject, void*, gd::PlayerObject*)
PROFILER_HOOK(GD + 0xd19b0, void, thiscall, gd::GameObject::deactivateObject, void*, bool)

PROFILER_HOOK(GD + 0xc9d30, bool, thiscall, gd::GameManager::getGameVariable, void*, const char*)
PROFILER_HOOK(GD + 0xc9b50, void, thiscall, gd::GameManager::setGameVariable, void*, const char*, bool)

// gd draw
PROFILER_HOOK(GD + 0x16ce90, void, thiscall, gd::DrawGridLayer::draw, void*)
PROFILER_HOOK(GD + 0x18fbe0, void, thiscall, gd::EditorUI::draw, void*)
PROFILER_HOOK(GD + 0x16b7c0, void, thiscall, gd::LevelEditorLayer::draw, void*)

// loading
PROFILER_HOOK(GD + 0x18c8e0, void, thiscall, gd::LoadingLayer::loadAssets, void*)
PROFILER_HOOK(COCOS2DSYM("?addImage@CCTextureCache@cocos2d@@QAEPAVCCTexture2D@2@PBD_N@Z"), CCTexture2D*, thiscall, cocos2d::CCTextureCache::addImage, void*, const char*, bool)
PROFILER_HOOK(COCOS2DSYM("?addImageAsync@CCTextureCache@cocos2d@@QAEXPBDPAVCCObject@2@P832@AEX1@Z@Z"), CCTexture2D*, thiscall, cocos2d::CCTextureCache::addImageAsync, void*, const char*, CCObject*, SEL_CallFuncO)
PROFILER_HOOK(COCOS2D + 0xec7c0, void*, c_decl, cocos2d::CCTextureCache::loadImage, void*)
PROFILER_HOOK(COCOS2D + 0xec9a0, void, c_decl, cocos2d::CCTextureCache::loadImageData, void*)
PROFILER_HOOK(COCOS2DSYM("?addSpriteFramesWithDictionary@CCSpriteFrameCache@cocos2d@@AAEXPAVCCDictionary@2@PAVCCTexture2D@2@@Z"), void, thiscall, cocos2d::CCSpriteFrameCache::addSpriteFramesWithDictionary, void*, CCDictionary*, CCTexture2D*)
PROFILER_HOOK(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBD0@Z"), void, thiscall, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, void*, const char*, const char*)
PROFILER_HOOK(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBD@Z"), void, thiscall, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, void*, const char*)
PROFILER_HOOK(COCOS2DSYM("?addSpriteFramesWithFile@CCSpriteFrameCache@cocos2d@@QAEXPBDPAVCCTexture2D@2@@Z"), void, thiscall, cocos2d::CCSpriteFrameCache::addSpriteFramesWithFile, void*, const char*, CCTexture2D*)
PROFILER_HOOK(COCOS2DSYM("?create@CCLabelBMFont@cocos2d@@SAPAV12@PBD0MW4CCTextAlignment@2@VCCPoint@2@@Z"), CCLabelBMFont*, c_decl, cocos2d::CCLabelBMFont::create, const char*, const char*, float, CCTextAlignment, CCPoint)
PROFILER_HOOK(GD + 0x25520, void, thiscall, gd::GameSoundManager::asynchronousSetup, void*)
PROFILER_HOOK(GD + 0xc9420, void, thiscall, gd::GameManager::FUN_0049420, void*)
PROFILER_HOOK(GD + 0xa8cb0, void, thiscall, gd::GameLevelManager::FUN_004a8cb0, void*)
PROFILER_HOOK(COCOS2DSYM("?initWithImage@CCTexture2D@cocos2d@@QAE_NPAVCCImage@2@@Z"), bool, thiscall, cocos2d::CCTexture2D::initWithImage, void*, CCImage*)
PROFILER_HOOK(COCOS2DSYM("?create@CCBMFontConfiguration@cocos2d@@SAPAV12@PBD@Z"), CCBMFontConfiguration*, c_decl, cocos2d::CCBMFontConfiguration::create, const char*)
PROFILER_HOOK(COCOS2DSYM("?parseConfigFile@CCBMFontConfiguration@cocos2d@@AAEPAV?$set@IU?$less@I@std@@V?$allocator@I@2@@std@@PBD@Z"), void*, thiscall, cocos2d::CCBMFontConfiguration::parseConfigFile, void*, const char*)
PROFILER_HOOK(COCOS2DSYM("?initWithTexture@CCSpriteBatchNode@cocos2d@@QAE_NPAVCCTexture2D@2@I@Z"), bool, thiscall, cocos2d::CCSpriteBatchNode::initWithTexture, void*, CCTexture2D*, unsigned int)
PROFILER_HOOK(COCOS2DSYM("?initWithTexture@CCSprite@cocos2d@@UAE_NPAVCCTexture2D@2@ABVCCRect@2@_N@Z"), bool, thiscall, cocos2d::CCSprite::initWithTexture, void*, CCTexture2D*, CCRect*, bool)
PROFILER_HOOK(COCOS2DSYM("?initWithTexture@CCTextureAtlas@cocos2d@@QAE_NPAVCCTexture2D@2@I@Z"), bool, thiscall, cocos2d::CCTextureAtlas::initWithTexture, void*, CCTexture2D*, unsigned int)
PROFILER_HOOK(COCOS2DSYM("?setupIndices@CCTextureAtlas@cocos2d@@AAEXXZ"), void, thiscall, cocos2d::CCTextureAtlas::setupIndices, void*)
PROFILER_HOOK(COCOS2DSYM("?mapBuffers@CCTextureAtlas@cocos2d@@AAEXXZ"), void, thiscall, cocos2d::CCTextureAtlas::mapBuffers, void*)

PROFILER_HOOK(COCOS2DSYM("?update@CCScheduler@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCScheduler::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCAccelerometer@cocos2d@@QAEXNNNN@Z"), void, membercall, cocos2d::CCAccelerometer::update, void*, double, double, double, double)
PROFILER_HOOK(COCOS2DSYM("?update@CCTimer@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCTimer::update, void*, float)

// actions
PROFILER_HOOK(COCOS2DSYM("?update@CCActionManager@cocos2d@@MAEXM@Z"), void, membercall, cocos2d::CCActionManager::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCTargetedAction@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCTargetedAction::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCActionEase@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCActionEase::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCActionTween@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCActionTween::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCAnimate@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCAnimate::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCCallFunc@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCCallFunc::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCSequence@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCSequence::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCBezierBy@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCBezierBy::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCBlink@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCBlink::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCCardinalSplineTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCCardinalSplineTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFadeIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFadeIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFadeOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFadeOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFadeOutTRTiles@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFadeOutTRTiles::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFadeTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFadeTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFlipX@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFlipX::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCFlipY@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCFlipY::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCJumpBy@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCJumpBy::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCMoveBy@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCMoveBy::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCPlace@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCPlace::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCRemoveSelf@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCRemoveSelf::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCRepeat@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCRepeat::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCReverseTime@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCReverseTime::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCRotateBy@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCRotateBy::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCRotateTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCRotateTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCScaleTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCScaleTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCSpawn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCSpawn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCTintBy@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCTintBy::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCTintTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCTintTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCToggleVisibility@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCToggleVisibility::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCHide@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCHide::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCShow@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCShow::update, void*, float)

// progress
PROFILER_HOOK(COCOS2DSYM("?update@CCProgressFromTo@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCProgressFromTo::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?updateBar@CCProgressTimer@cocos2d@@IAEXXZ"), void, thiscall, cocos2d::CCProgressTimer::updateBar, void*)
PROFILER_HOOK(COCOS2DSYM("?updateColor@CCProgressTimer@cocos2d@@QAEXXZ"), void, thiscall, cocos2d::CCProgressTimer::updateColor, void*)
PROFILER_HOOK(COCOS2DSYM("?updateProgress@CCProgressTimer@cocos2d@@IAEXXZ"), void, thiscall, cocos2d::CCProgressTimer::updateProgress, void*)
PROFILER_HOOK(COCOS2DSYM("?updateRadial@CCProgressTimer@cocos2d@@IAEXXZ"), void, thiscall, cocos2d::CCProgressTimer::updateRadial, void*)

// easings
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBackIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBackIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBackInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBackInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBackOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBackOut::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBounceIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBounceIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBounceInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBounceInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseBounceOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseBounceOut::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCEaseElasticIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseElasticIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseElasticInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseElasticInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseElasticOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseElasticOut::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCEaseExponentialIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseExponentialIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseExponentialInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseExponentialInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseExponentialOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseExponentialOut::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCEaseIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseOut::update, void*, float)

PROFILER_HOOK(COCOS2DSYM("?update@CCEaseSineIn@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseSineIn::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseSineInOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseSineInOut::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?update@CCEaseSineOut@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCEaseSineOut::update, void*, float)

// particles
PROFILER_HOOK(COCOS2DSYM("?draw@CCParticleBatchNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCParticleBatchNode::draw, void*)
PROFILER_HOOK(COCOS2DSYM("?visit@CCParticleBatchNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCParticleBatchNode::visit, void*)
PROFILER_HOOK(COCOS2DSYM("?update@CCParticleSystem@cocos2d@@UAEXM@Z"), void, membercall, cocos2d::CCParticleSystem::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?updateWithNoTime@CCParticleSystem@cocos2d@@UAEXXZ"), void, membercall, cocos2d::CCParticleSystem::updateWithNoTime, void*)
PROFILER_HOOK(COCOS2DSYM("?draw@CCParticleSystemQuad@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCParticleSystemQuad::draw, void*)

// sprites
PROFILER_HOOK(COCOS2DSYM("?setOpacity@CCSprite@cocos2d@@UAEXE@Z"), void, thiscall, cocos2d::CCSprite::setOpacity, void*, GLubyte)
PROFILER_HOOK(COCOS2DSYM("?setColor@CCSprite@cocos2d@@UAEXABU_ccColor3B@2@@Z"), void, thiscall, cocos2d::CCSprite::setColor, void*, const ccColor3B&)
PROFILER_HOOK(COCOS2DSYM("?setOpacityModifyRGB@CCSprite@cocos2d@@UAEX_N@Z"), void, thiscall, cocos2d::CCSprite::setOpacityModifyRGB, void*, bool)
PROFILER_HOOK(COCOS2DSYM("?updateDisplayedColor@CCSprite@cocos2d@@UAEXABU_ccColor3B@2@@Z"), void, thiscall, cocos2d::CCSprite::updateDisplayedColor, void*, const ccColor3B&)
PROFILER_HOOK(COCOS2DSYM("?updateDisplayedOpacity@CCSprite@cocos2d@@UAEXE@Z"), void, thiscall, cocos2d::CCSprite::updateDisplayedOpacity, void*, GLubyte)
PROFILER_HOOK(COCOS2DSYM("?updateColor@CCSprite@cocos2d@@IAEXXZ"), void, thiscall, cocos2d::CCSprite::updateColor, void*)
PROFILER_HOOK(COCOS2DSYM("?updateTransform@CCSprite@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCSprite::updateTransform, void*)
PROFILER_HOOK(COCOS2DSYM("?draw@CCSprite@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCSprite::draw, void*)
PROFILER_HOOK(COCOS2DSYM("?displayFrame@CCSprite@cocos2d@@UAEPAVCCSpriteFrame@2@XZ"), CCSpriteFrame*, thiscall, cocos2d::CCSprite::displayFrame, void*)
PROFILER_HOOK(COCOS2DSYM("?sortAllChildren@CCSprite@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCSprite::sortAllChildren, void*)
PROFILER_HOOK(COCOS2DSYM("?draw@CCSpriteBatchNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCSpriteBatchNode::draw, void*)
PROFILER_HOOK(COCOS2DSYM("?sortAllChildren@CCSpriteBatchNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCSpriteBatchNode::sortAllChildren, void*)
PROFILER_HOOK(COCOS2DSYM("?updateAtlasIndex@CCSpriteBatchNode@cocos2d@@AAEXPAVCCSprite@2@PAH@Z"), void, thiscall, cocos2d::CCSpriteBatchNode::updateAtlastIndex, void*, CCSprite*, int*)
PROFILER_HOOK(COCOS2DSYM("?swap@CCSpriteBatchNode@cocos2d@@AAEXHH@Z"), void, thiscall, cocos2d::CCSpriteBatchNode::swap, void*, int, int)

PROFILER_HOOK(COCOS2DSYM("?setString@CCLabelBMFont@cocos2d@@UAEXPBD_N@Z"), void, thiscall, cocos2d::CCLabelBMFont::setString, void*, const char*, bool)
PROFILER_HOOK(COCOS2DSYM("?updateLabel@CCLabelBMFont@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCLabelBMFont::updateLabel, void*)

PROFILER_HOOK(COCOS2DSYM("?updateQuad@CCTextureAtlas@cocos2d@@QAEXPAU_ccV3F_C4B_T2F_Quad@2@I@Z"), void, thiscall, cocos2d::CCTextureAtlas::updateQuad, void*, ccV3F_C4B_T2F_Quad*, unsigned int)

PROFILER_HOOK(COCOS2DSYM("?update@CCNode@cocos2d@@UAEXM@Z"), void, thiscall, cocos2d::CCNode::update, void*, float)
PROFILER_HOOK(COCOS2DSYM("?visit@CCNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCNode::visit, void*)
PROFILER_HOOK(COCOS2DSYM("?updateTransform@CCNode@cocos2d@@UAEXXZ"), void, thiscall, cocos2d::CCNode::updateTransform, void*)

// post frame? haven't actually seen this getting called so idk
PROFILER_HOOK(COCOS2DSYM("?updateVerticalSync@CCApplication@cocos2d@@QAEXXZ"), void, thiscall, cocos2d::CCApplication::updateVerticalSync, void*)
PROFILER_HOOK(COCOS2DSYM("?updateControllerKeys@CCApplication@cocos2d@@QAEXXZ"), void, thiscall, cocos2d::CCApplication::updateControllerKeys, void*)
