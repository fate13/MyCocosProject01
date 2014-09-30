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
#include "CustomPhysicsSprite.h"
#include "ui/CocosGUI.h"


USING_NS_CC;

class GameMainScene : public CustomLayer
{
public:

    static Scene* createScene();

	CREATE_FUNC(GameMainScene);

	virtual void onEnterTransitionDidFinish() override;

	virtual void update(float dt);

	GameMainScene();
	virtual ~GameMainScene() override;

	Vector<CustomPhysicsSprite*> getBallsList() const;

	ui::ImageView* getDust() const;



protected:

	virtual void sceneSetting(const std::string imageName) override;
	
	
private:

	std::string textureAtlasName;

	enum class ZOrder
	{
		Batch,
		BG,
		Dust,
		Ball,
		BallBlur,
		ControllLayer,
		CountDown,
		PresentPoints,
		TotalPoints,
		Timer
	};

	enum class Tag
	{
		BG,
		Dust,
		Ball,
		CountDown,
		ControllLayer,
		PresentPoints,
		TotalPoints,
		Timer,
		Ready,
		Go,
		Timeup
	};

	b2World* _world;
	
	Vector<CustomPhysicsSprite*> _ballsList;

	Vector<CustomPhysicsSprite*> _destroyBallsList;
	
	//SpriteBatchNode* _gameMainBatchNode;
	//Layer* ll;

	std::shared_ptr<GameMainUserController> _userController;

	bool _dustSelectedFlag;

	void initPhysics();
	void createBallsInInit();
	void createBG();
	void createBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo);

	void createPauseButton();
	void startGame();
	void createCountDown();
	void openCountDown(float dt);
	void setEventListener();
	void checkTime();
	void checkBalls();
	void checkDust();
	void checkPoints();
	void pushDestroyBallsList(const std::shared_ptr<GameMainModel::BallInfo> ballInfo);
	void setDestroyBallsAnimation();
	void destroyBall(CustomPhysicsSprite* ball);
	void changeTextureOfBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo, const CustomPhysicsSprite::BallTextureType ballTextureType);
	bool _pauseFlag;
	bool _startTimerFlag;

	void touchEvent_pauseButton(Ref* pSender, ui::Widget::TouchEventType type);

};

#endif /* defined(__MyCocosProject01__GameMainScene__) */
