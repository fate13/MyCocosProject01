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
#include <math.h>
#include <vector>

GameMainUserController::~GameMainUserController()
{
	CCLOG("delete");
}

GameMainUserController::GameMainUserController()
{
	_model = std::make_shared<GameMainModel>();
}

std::shared_ptr<GameMainModel> GameMainUserController::getModel() const
{
	return _model;
}

bool GameMainUserController::touchBegan(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	const GameMainScene* view = static_cast<GameMainScene*>(obj);

	Node* target = event->getCurrentTarget();
	Vec2 touchPoint = target->convertTouchToNodeSpace(touch);

	for (CustomPhysicsSprite* ball : view->getBallsList()) {

		// removeしたballも検知してしまうので、isPresenceではじく
		if (!ball->isPresence())
			continue;

		if (circleContainPoint(ball, touchPoint)) {
			_model->touchBall(ball->getId());
			return true;
		}

	}

	return false;
}

void GameMainUserController::touchMoved(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	const GameMainScene* view = static_cast<GameMainScene*>(obj);

	Node* target = event->getCurrentTarget();
	Vec2 touchPoint = target->convertTouchToNodeSpace(touch);

	if (circleContainPoint(view->getDust(), touchPoint)) {
		_model->enterDust();
		return;
	}

	_model->outDust();

	for (CustomPhysicsSprite* ball : view->getBallsList()) {

		// removeしたballも検知してしまうので、isPresenceではじく
		if (!ball->isPresence())
			continue;

		if (circleContainPoint(ball, touchPoint)) {
			_model->touchBall(ball->getId());
			return;
		}

	}

}

void GameMainUserController::touchEnded(cocos2d::Touch *touch, cocos2d::Event *event, Node* obj)
{
	const GameMainScene* view = static_cast<GameMainScene*>(obj);

	Node* target = event->getCurrentTarget();
	Vec2 touchPoint = target->convertTouchToNodeSpace(touch);

	_model->releaseBall();
}

void GameMainUserController::sendDtAndBalls(const float dt, const Vector<CustomPhysicsSprite*> ballsList)
{
	std::vector<std::pair<float, float>> ballPosition;
	int size = static_cast<int>(ballsList.size());

	for (int i = 0; i < size; ++i)
	{
		ballPosition.push_back(std::make_pair(ballsList.at(i)->getPosition().x, ballsList.at(i)->getPosition().y));
	}

	_model->receiveDtAndBallsPosition(dt, ballPosition);
}


bool GameMainUserController::circleContainPoint(Node* circle, Vec2 touchPoint)
{
	float cX = circle->getPosition().x;
	float cY = circle->getPosition().y;
	float r = circle->getContentSize().width / 2;
	float tX = touchPoint.x;
	float tY = touchPoint.y;

	if ( powf((tX-cX), 2) + powf((tY-cY), 2) <= powf(r, 2) )
		return true;
	else
		return false;
}












