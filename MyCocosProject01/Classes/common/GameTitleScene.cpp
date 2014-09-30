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
#include "cocostudio/CocoStudio.h"


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
		createUI();
	}

	pleaseWaitLayer_out(nullptr);

}

void GameTitleScene::createUI()
{
	this->addChild( cocostudio::GUIReader::getInstance()->widgetFromJsonFile("GameTitleScene.ExportJson") );

	ui::Button* playButton = dynamic_cast<ui::Button*>( this->getChildByName("GameTitleScene_UI")->getChildByName("GameTitleScene_frontContainer")->getChildByName("GameTitleScene_playButton") );
	playButton->setPressedActionEnabled(true);
	playButton->addTouchEventListener(CC_CALLBACK_2(GameTitleScene::touchEvent_playButton, this));

	ui::Button* howtoButton = dynamic_cast<ui::Button*>( this->getChildByName("GameTitleScene_UI")->getChildByName("GameTitleScene_frontContainer")->getChildByName("GameTitleScene_howtoButton") );
	howtoButton->setTouchEnabled(false);
	howtoButton->setBright(false);

	ui::Button* scoreButton = dynamic_cast<ui::Button*>( this->getChildByName("GameTitleScene_UI")->getChildByName("GameTitleScene_frontContainer")->getChildByName("GameTitleScene_scoreButton") );
	scoreButton->setTouchEnabled(false);
	scoreButton->setBright(false);

}

void GameTitleScene::touchEvent_playButton(Ref* pSender, ui::Widget::TouchEventType type)
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
			dynamic_cast<ui::Button*>(pSender)->setPressedActionEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setTouchEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setHighlighted(true);
			gotoNextScene(GameMainScene::createScene());
            break;

        case ui::Widget::TouchEventType::CANCELED:
			CCLOG("Touch Cancelled");
            break;

        default:
            break;
    }
}



