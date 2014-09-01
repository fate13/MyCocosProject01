//
//  GameMainScene.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#include "GameMainScene.h"
#include "GameTitleScene.h"

GameMainScene::~GameMainScene()
{
	//dispose("GameMainScene");
	CCLOG("GameMain dispose");
}

Scene* GameMainScene::createScene()
{
	GameMainScene* instance = GameMainScene::create();
	Scene* scene = Scene::create();
	scene->addChild(instance);
	return scene;
}

void GameMainScene::onEnterTransitionDidFinish()
{
	//loadTextureAtlasAsync("GameTitleScene");
	sceneSetting("GameMainScene");
}

void GameMainScene::sceneSetting(const std::string imageName)
{

	pleaseWaitLayer_out();

	LayerColor* overLayer = LayerColor::create(Color4B(255, 0, 0, 128));
	this->addChild(overLayer);

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [this](Touch *touch, Event *event)
	{
		this->gotoNextScene("GameTitleScene");
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, overLayer);

}

void GameMainScene::replaceScene(const std::string nextSceneName)
{
	if (nextSceneName == "GameTitleScene") {
		Director::getInstance()->replaceScene(GameTitleScene::createScene());
	}
}
