//
//  GameTitleScene.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#include "AppMacros.h"
#include "GameTitleScene.h"

Scene* GameTitleScene::createScene()
{
	GameTitleScene* instance = GameTitleScene::create();
	Scene* scene = Scene::create();

	scene->addChild(instance);

	return scene;
}

bool GameTitleScene::init()
{
	if (!Layer::init())
	{
		return false;
	}

	createBG();

	return true;
}

void GameTitleScene::createBG()
{
	Sprite* bg = Sprite::create("testBG.png");
	bg->setPosition(WIN_CENTER);

	this->addChild(bg);
}