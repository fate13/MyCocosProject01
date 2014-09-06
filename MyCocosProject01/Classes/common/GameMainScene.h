//
//  GameMainScene.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#ifndef __MyCocosProject01__GameMainScene__
#define __MyCocosProject01__GameMainScene__

#include "cocos2d.h"
#include "CustomLayer.h"
#include "BOX2D/BOX2D.h"
#include "GameMainUserController.h"


USING_NS_CC;

class GameMainScene : public CustomLayer
{
private:

	SpriteBatchNode* gameMainBatchNode;

	std::shared_ptr<GameMainUserController> userController;

	void initPhysics();
	void createBG();
	void createBall();
	void createReturnButton();
	void startGame();
	void setEventListener();

protected:

	virtual void sceneSetting(const std::string imageName) override;


public:

	b2World* world;
	
    static Scene* createScene();

	CREATE_FUNC(GameMainScene);

	virtual void onEnterTransitionDidFinish() override;

	GameMainScene();
	virtual ~GameMainScene() override;

	virtual void update(float dt);
	
};

#endif /* defined(__MyCocosProject01__GameMainScene__) */
