//
//  GameMainScene.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#include "AppMacros.h"
#include "GameMainScene.h"
#include "GameTitleScene.h"

GameMainScene::~GameMainScene()
{
	dispose("GameMainScene");
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
	loadTextureAtlasAsync("GameMainScene");
}

void GameMainScene::sceneSetting(const std::string imageName)
{
	if (imageName == "GameMainScene") {
		initPhysics();
		createBG();
	}

	pleaseWaitLayer_out();
}

void GameMainScene::initPhysics()
{

}

void GameMainScene::createBG()
{
	Sprite* bg = Sprite::createWithSpriteFrameName("BG.png");
	bg->setPosition(WIN_POS(0.5, 0.5));
	this->addChild(bg);


}



