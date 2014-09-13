//
//  GameTitleScene.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#include "AppMacros.h"
#include "GameTitleScene.h"
#include "GameMainScene.h"


GameTitleScene::~GameTitleScene()
{
	dispose(textureAtlasName);
}

GameTitleScene::GameTitleScene()
{
	textureAtlasName = "GameTitleScene";
}

Scene* GameTitleScene::createScene()
{
	GameTitleScene* instance = GameTitleScene::create();
	Scene* scene = Scene::create();
	scene->addChild(instance);
	return scene;
}

void GameTitleScene::onEnterTransitionDidFinish()
{
	loadTextureAtlasAsync(textureAtlasName);
}

void GameTitleScene::sceneSetting(const std::string imageName)
{
	if (imageName == textureAtlasName)
	{
		createBG();
		createStartButton();
	}

	pleaseWaitLayer_out(nullptr);

}

void GameTitleScene::createBG()
{
	Sprite* bg = Sprite::createWithSpriteFrameName("testBG.png");
	bg->setPosition(WIN_POS(0.5f, 0.5f));

	this->addChild(bg);
}

void GameTitleScene::createStartButton()
{
	ui::Button* button = ui::Button::create("startButton_normal.png", "startButton_pressed.png", "startButton_disabled.png", ui::TextureResType::PLIST);
	button->setPressedActionEnabled(true);
	button->setPosition(WIN_POS(0.5f, 0.5f));
	button->addTouchEventListener(CC_CALLBACK_2(GameTitleScene::touchEvent_startButton, this));

	ui::Layout* layout = ui::Layout::create();
	this->addChild(layout);
	layout->addChild(button);

}

void GameTitleScene::touchEvent_startButton(Ref *pSender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            CCLOG("Touch Down");
            break;

        case ui::Widget::TouchEventType::MOVED:
            CCLOG("Touch Move");
            break;

        case ui::Widget::TouchEventType::ENDED:
            CCLOG("Touch Up");
			gotoNextScene(GameMainScene::createScene());
            break;

        case ui::Widget::TouchEventType::CANCELED:
			CCLOG("Touch Cancelled");
            break;

        default:
            break;
    }
}



