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
#include "cocostudio/CocoStudio.h"
#include <limits.h>

CustomLayer::~CustomLayer()
{
	//dispose("CustomLayer");
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

	return true;
}

void CustomLayer::onEnterTransitionDidFinish()
{
	loadTextureAtlasAsync("CustomLayer");
}

void CustomLayer::pleaseWaitLayer_set()
{
	Layer* overLayer = Layer::create();
	this->addChild(overLayer, std::numeric_limits<int>::max(), "pleaseWaitLayer");

	overLayer->addChild( cocostudio::GUIReader::getInstance()->widgetFromJsonFile("Common.ExportJson") );

	cocostudio::ActionManagerEx::getInstance()->playActionByName("Common.ExportJson", "pleaseWait_loop");

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch *touch, Event *event)
	{
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overLayer);
}

void CustomLayer::pleaseWaitAnimation_loop(Layer* overLayer)
{

}

void CustomLayer::pleaseWaitLayer_out(const std::function<void()> callBackFunc)
{
	cocostudio::ActionManagerEx::getInstance()->getActionByName("Common.ExportJson", "pleaseWait_loop")->stop();

	Layer* overLayer = static_cast<Layer*>( this->getChildByName("pleaseWaitLayer") );

	std::function<void()> func = [overLayer, callBackFunc](){
		Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(overLayer);
		overLayer->setVisible(false);

		if (callBackFunc) {
			callBackFunc();
		}
	};

	CallFunc* callFunc = CallFunc::create(func);

	cocostudio::ActionManagerEx::getInstance()->playActionByName("Common.ExportJson", "pleaseWait_out", callFunc);
}

void CustomLayer::pleaseWaitAnimation_out(const Layer* overLayer, const std::function<void()> callBackFunc)
{

}


void CustomLayer::pleaseWaitLayer_in(const std::function<void()> callBackFunc)
{
	Layer* overLayer = static_cast<Layer*>( this->getChildByName("pleaseWaitLayer") );

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch *touch, Event *event)
	{
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overLayer);

	overLayer->setVisible(true);

	CallFunc* callFunc = CallFunc::create(callBackFunc);

	cocostudio::ActionManagerEx::getInstance()->playActionByName("Common.ExportJson", "pleaseWait_in", callFunc);
}

void CustomLayer::pleaseWaitAnimation_in(const Layer* overLayer, const std::function<void()> callBackFunc)
{
	Sprite* img = (Sprite*)overLayer->getChildByName("img");
	img->stopAllActions();
	img->runAction(PleaseWaitAnimation::inAnimation(callBackFunc));
}

void CustomLayer::loadTextureAtlasAsync(const std::string imageName)
{
	pleaseWaitLayer_set();

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

