//
//  CustomLayer.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/01.
//
//

#include "CustomLayer.h"
#include "AppMacros.h"
#include "PleaseWaitAnimation.h"

CustomLayer::~CustomLayer()
{
	dispose("CustomLayer");
}

Scene* CustomLayer::createScene()
{
	CustomLayer* instance = CustomLayer::create();
	Scene* scene = Scene::create();
	scene->addChild(instance);
	return scene;
}

bool CustomLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	pleaseWaitLayer_set();

	return true;
}

void CustomLayer::onEnterTransitionDidFinish()
{
	loadTextureAtlasAsync("CustomLayer");
}

void CustomLayer::pleaseWaitLayer_set()
{
	LayerColor* overLayer = LayerColor::create(Color4B(0, 0, 0, 0));
	this->addChild(overLayer, 256, "pleaseWaitLayer");

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch *touch, Event *event)
	{
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overLayer);

	pleaseWaitAnimation_loop(overLayer);
}

void CustomLayer::pleaseWaitAnimation_loop(LayerColor* overLayer)
{
	Sprite* img = Sprite::createWithSpriteFrameName("HelloWorld.png");
	img->setName("img");
	img->setPosition(WIN_POS(0.5f, 0.5f));
	img->runAction(PleaseWaitAnimation::loopAnimation());
	overLayer->addChild(img);
}

void CustomLayer::pleaseWaitLayer_out(std::function<void()> callBackFunc)
{
	LayerColor* overLayer = static_cast<LayerColor*>( this->getChildByName("pleaseWaitLayer") );

	std::function<void()> func = [overLayer, callBackFunc](){
		Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(overLayer);
		overLayer->setVisible(false);

		if (callBackFunc) {
			callBackFunc();
		}
	};

	pleaseWaitAnimation_out(overLayer, func);
}

void CustomLayer::pleaseWaitAnimation_out(LayerColor* overLayer, std::function<void()> callBackFunc)
{
	Sprite* img = (Sprite*)overLayer->getChildByName("img");
	img->stopAllActions();
	img->runAction(PleaseWaitAnimation::outAnimation(callBackFunc));
}


void CustomLayer::pleaseWaitLayer_in(std::function<void()> callBackFunc)
{
	LayerColor* overLayer = static_cast<LayerColor*>( this->getChildByName("pleaseWaitLayer") );

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch *touch, Event *event)
	{
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overLayer);

	overLayer->setVisible(true);

	pleaseWaitAnimation_in(overLayer, callBackFunc);
}

void CustomLayer::pleaseWaitAnimation_in(LayerColor* overLayer, std::function<void()> callBackFunc)
{
	Sprite* img = (Sprite*)overLayer->getChildByName("img");
	img->stopAllActions();
	img->runAction(PleaseWaitAnimation::inAnimation(callBackFunc));
}

void CustomLayer::loadTextureAtlasAsync(const std::string imageName)
{
	Director::getInstance()->getTextureCache()->addImageAsync(IMAGE_NAME(imageName), CC_CALLBACK_1(CustomLayer::loadTextureAtlasAsyncComplete, this, imageName));
}

void CustomLayer::loadTextureAtlasAsyncComplete(Texture2D* texture, const std::string imageName)
{
	SpriteFrameCache::getInstance()->addSpriteFramesWithFile( (imageName + ".plist"), texture);
	sceneSetting(imageName);
}

void CustomLayer::sceneSetting(const std::string imageName)
{
	if (imageName == "CustomLayer")
	{

	}
	pleaseWaitLayer_out(nullptr);
	dispose("CustomLayer");
}

void CustomLayer::gotoNextScene(Scene* nextScene)
{
	_nextScene = nextScene;

	// ここでretainしておかないと、アニメーションの最中に解放されてしまう。
	_nextScene->retain();

	pleaseWaitLayer_in(CC_CALLBACK_0(CustomLayer::replaceScene, this));
}

void CustomLayer::replaceScene()
{
	// gotoNextScene関数内でretainしたので、releaseする。ただし最初のSceneはretainしていないので例外。
	if(Director::getInstance()->getRunningScene()->getReferenceCount() > 2)
	{
		Director::getInstance()->getRunningScene()->release();
	}
	
	Director::getInstance()->replaceScene(_nextScene);
}

void CustomLayer::dispose(const std::string imageName)
{
	SpriteFrameCache::getInstance()->removeSpriteFramesFromFile((imageName + ".plist"));
	Director::getInstance()->getTextureCache()->removeTextureForKey(IMAGE_NAME(imageName));
}

