#include "GameConfig.h"
#include "AppDelegate.h"
#include "AppMacros.h"
#include "common/GameTitleScene.h"

USING_NS_CC;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }


	// デザインサイズの指定
	director->getOpenGLView()->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);

	// リソースのマルチリゾリューション対応
	// iPad用リソース
	if (glview->getFrameSize().height > largeResourceThreshold)
	{
		director->setContentScaleFactor(largeResource.size.height / designResolutionSize.height);
		FileUtils::getInstance()->addSearchPath(largeResource.directory);
	}
	// iPhoneHD用リソース
	else if (glview->getFrameSize().height > smallResource.size.height)
	{
		director->setContentScaleFactor(mediumResource.size.height / designResolutionSize.height);
		FileUtils::getInstance()->addSearchPath(mediumResource.directory);
	}
	// iPhone用リソース
	else
	{
		director->setContentScaleFactor(smallResource.size.height / designResolutionSize.height);
		FileUtils::getInstance()->addSearchPath(smallResource.directory);
	}

	FileUtils::getInstance()->addSearchPath("JsonAnimation");

	FileUtils::getInstance()->addSearchPath("fonts");


	// turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(APP_FPS);

	// 各シーン共通使用のテクスチャを事前に読み込んでおく
	std::string imageName = "Common";
	Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(IMAGE_NAME(imageName));
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile((imageName + ".plist"), texture);

    // create a scene. it's an autorelease object
    auto scene = GameTitleScene::createScene();

	// run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
