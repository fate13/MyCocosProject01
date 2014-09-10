//
//  GameMainUserController.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/05.
//
//

#ifndef __MyCocosProject01__GameMainUserController__
#define __MyCocosProject01__GameMainUserController__

#include "cocos2d.h"
#include "GameMainModel.h"
#include "CustomPhysicsSprite.h"


USING_NS_CC;

class GameMainUserController
{
public:

	// 第3引数はGameMainSceneクラスだが、includeの関係で、ここではNodeで指定しておく
	bool touchBegan(Touch* touch, Event* event, Node* obj);
	void touchMoved(Touch* touch, Event* event, Node* obj);
	void touchEnded(Touch* touch, Event* event, Node* obj);
	void sendDtAndBalls(const float dt, const Vector<CustomPhysicsSprite*> ballList);
	void setCompleteVanishAnimation();

	std::shared_ptr<GameMainModel> getModel() const;

	GameMainUserController();
	~GameMainUserController();


private:
	
	std::shared_ptr<GameMainModel> _model;

	bool circleContainPoint(Sprite* circle, Vec2 touchPoint);
};

#endif /* defined(__MyCocosProject01__GameMainUserController__) */
