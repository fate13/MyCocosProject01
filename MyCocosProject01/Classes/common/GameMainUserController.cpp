//
//  GameMainUserController.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/05.
//
//

#include "GameMainUserController.h"
#include "GameMainScene.h"
#include "BOX2D/BOX2D.h"

GameMainUserController::~GameMainUserController()
{
	CCLOG("delete");
}

bool GameMainUserController::touchBegan(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	const GameMainScene* view = static_cast<GameMainScene*>(obj);

	Node* target = event->getCurrentTarget();
	Vec2 touchPoint = target->convertTouchToNodeSpace(touch);

	for (b2Body* body = view->world->GetBodyList(); body; body = body->GetNext()) {

		if (body->GetType() != b2_dynamicBody)
			continue;

		Sprite* ball = static_cast<Sprite*>(body->GetUserData());

		// Spriteのpositionはアンカーポイント基準なので注意！
		Rect ballRect = Rect(ball->getPosition().x-ball->getContentSize().width / 2, ball->getPosition().y - ball->getContentSize().height / 2, ball->getContentSize().width, ball->getContentSize().height);

		if (ballRect.containsPoint(touchPoint)) {
			view->world->DestroyBody(body);
			ball->removeFromParentAndCleanup(true);

			return true;
		}


	}

	return false;
}

void GameMainUserController::touchMoved(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	CCLOG("move");
}

void GameMainUserController::touchEnded(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	CCLOG("end");
}