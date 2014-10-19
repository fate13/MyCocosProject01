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
		UI,
		Ball,
	};

	enum class Tag
	{
		Ball,
		DustBlur,
		TimerBG,
		Pentagon1,
		Pentagon2,
		ChainUIBlink
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
	void createUI();
	void createBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo);

	void createPauseButton();
	void createContinueButton();
	void createScoreButton();
	void createRetryButton();

	void startGame();
	void createCountDown();
	void openCountDown();
	void setEventListener();
	void checkTime();
	void checkBalls();
	void drawLineOnSelectedBalls();
	void checkDust();
	void animationDustBlur();
	void checkPoints();
	void updateTotalPoints(float dt);
	void checkChainPoints();
	void setBonusModeIn();
	void setBonusModeOut();
	void DelaySetTotalPoints(int preTotalPoints);
	void pushDestroyBallsList(const std::shared_ptr<GameMainModel::BallInfo> ballInfo);
	void setDestroyBallsAnimation();
	void destroyBall(CustomPhysicsSprite* ball);
	void changeTextureOfBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo, const CustomPhysicsSprite::BallTextureType ballTextureType);
	void gamePause();
	void gameContinue();

	void setChainUI();

	bool _pauseFlag;
	bool _timeupFlag;
	bool _last10Flag;
	bool _bonusModeFlag;

	int _preAddingPoints;
	int _preTotalPoints;
	int _realyTotalPoints;

	int _preTimeLeft;

	void touchEvent_pauseButton(Ref* pSender, ui::Widget::TouchEventType type);
	void touchEvent_continueButton(Ref* pSender, ui::Widget::TouchEventType type);
	void touchEvent_scoreButton(Ref* pSender, ui::Widget::TouchEventType type);
	void touchEvent_retryButton(Ref* pSender, ui::Widget::TouchEventType type);

};

#endif /* defined(__MyCocosProject01__GameMainScene__) */
