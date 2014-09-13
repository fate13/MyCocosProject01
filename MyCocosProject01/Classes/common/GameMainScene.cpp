//
//  GameMainScene.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#include "GameConfig.h"
#include "AppMacros.h"
#include "GameConfig.h"
#include "GameMainScene.h"
#include "GameTitleScene.h"
#include "extensions/cocos-ext.h"
#include "physicsEditor/GB2ShapeCache-x.h"


GameMainScene::~GameMainScene()
{
	for (b2Body* body = _world->GetBodyList(); body; body = body->GetNext()) {
		_world->DestroyBody(body);
	}
	delete _world;
	_world = nullptr;
	
	dispose("GameMainScene");

	_ballsList.clear();
	_destroyBallsList.clear();
}

GameMainScene::GameMainScene()
{
	_userController = std::make_shared<GameMainUserController>();

	_dustSelectedFlag = false;
	_pauseFlag = false;
	_startTimerFlag = false;
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
		
		//Texture2D* texture = Director::getInstance()->getTextureCache()->getTextureForKey(IMAGE_NAME(imageName));
		//_gameMainBatchNode =SpriteBatchNode::createWithTexture(texture);
		//this->addChild(_gameMainBatchNode, static_cast<int>(ZOrder::Batch));

		initPhysics();
		createBG();
		createReturnButton();
		createDust();
		createBallsInInit();
		createPoints();
		createTimer();
	}

	pleaseWaitLayer_out(CC_CALLBACK_0(GameMainScene::startGame, this));
}

void GameMainScene::initPhysics()
{
	gbox2d::GB2ShapeCache::getInstance()->addShapesWithFile("Physics/GameMainScenePhysics.plist");

	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	_world = new b2World(gravity);
}

void GameMainScene::startGame()
{
	createCountDown();
	this->scheduleUpdate();
	setEventListener();
}

void GameMainScene::createCountDown()
{
	Layer* layer = Layer::create();
	this->addChild(layer, static_cast<int>(ZOrder::CountDown), static_cast<int>(Tag::CountDown));

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = [](Touch *touch, Event *event)
	{
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);

	this->scheduleOnce(schedule_selector(GameMainScene::openCountDown), 2);
}

void GameMainScene::openCountDown(float dt)
{
	Layer* layer = static_cast<Layer*>(this->getChildByTag(static_cast<int>(Tag::CountDown)));
	Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(layer);
	layer->removeFromParentAndCleanup(true);
	_startTimerFlag = true;
}

void GameMainScene::update(float dt)
{
	if (!_pauseFlag)
		_world->Step(APP_FPS, VELOCITY_ITERATION, POSITION_ITERATION);

	if (!_startTimerFlag)
		dt = 0.0f;
	
	_userController->sendDtAndBalls(dt, _ballsList);

	checkTime();
	checkBalls();
	checkDust();
	checkPoints();
}

void GameMainScene::setEventListener()
{
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameMainUserController::touchBegan, _userController, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameMainUserController::touchMoved, _userController, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameMainUserController::touchEnded, _userController, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void GameMainScene::checkTime()
{
	char time[100];
	Label* timer = static_cast<Label*>(this->getChildByTag(static_cast<int>(Tag::Timer)));
	sprintf(time, "%d", _userController->getModel()->getTimeLeft());
	timer->setString(time);


	if (_userController->getModel()->getGameStatus() == GameMainModel::GameStatus::TimeUp)
	{
		if(_userController->getModel()->getGamePrevStatus() == GameMainModel::GameStatus::Playing)
		{
			Layer* layer = Layer::create();
			this->addChild(layer);

			EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
			listener->setSwallowTouches(true);
			listener->onTouchBegan = [](Touch *touch, Event *event)
			{
				return true;
			};
			Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, layer);
		}
	}
}

void GameMainScene::checkBalls()
{
	if	(_userController->getModel()->getBallsInfoList().size() > _ballsList.size())
	{
		int addBallsNumber = _userController->getModel()->getBallsInfoList().size() - _ballsList.size();
		int size = _ballsList.size();
		for (int i = size; i < size + addBallsNumber; ++i) {
			createBall(_userController->getModel()->getBallsInfoList().at(i));
		}
	}

	for (std::shared_ptr<GameMainModel::BallInfo> ballInfo : _userController->getModel()->getBallsInfoList())
	{
//		if (ballInfo->isPresence == false)
//		{
//			pushDestroyBallsList(ballInfo);
//		}
		if (ballInfo->isSelected)
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Selected);
		}
		else if (!ballInfo->isSelectEnable)
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Disabled);
		}
		else
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Normal);
		}
	}

	setDestroyBallsAnimation();
}

void GameMainScene::checkDust()
{
	if (_dustSelectedFlag == _userController->getModel()->isDustSelected())
		return;

	_dustSelectedFlag = !_dustSelectedFlag;

	if (_dustSelectedFlag)
		static_cast<Sprite*>(this->getChildByTag(static_cast<int>(Tag::Dust)))->setSpriteFrame("dust_selected.png");
	else
		static_cast<Sprite*>(this->getChildByTag(static_cast<int>(Tag::Dust)))->setSpriteFrame("dust_normal.png");
}

void GameMainScene::createPoints()
{
	Label* presentPoints = Label::createWithSystemFont("0", "", 100);
	presentPoints->setPosition(WIN_POS(0.8, 0.9));
	presentPoints->setColor(Color3B(0, 0, 0));
	this->addChild(presentPoints, static_cast<int>(ZOrder::PresentPoints), static_cast<int>(Tag::PresentPoints));

	Label* totalPoints = Label::createWithSystemFont("0", "", 100);
	totalPoints->setPosition(WIN_POS(0.2, 0.9));
	totalPoints->setColor(Color3B(255, 255, 0));
	this->addChild(totalPoints, static_cast<int>(ZOrder::TotalPoints), static_cast<int>(Tag::TotalPoints));
}

void GameMainScene::checkPoints()
{
	char points[100];

	Label* presentPoints = static_cast<Label*>(this->getChildByTag(static_cast<int>(Tag::PresentPoints)));
	sprintf(points, "%d", _userController->getModel()->getPresentPoints());
	presentPoints->setString(points);

	Label* totalPoints = static_cast<Label*>(this->getChildByTag(static_cast<int>(Tag::TotalPoints)));
	sprintf(points, "%d", _userController->getModel()->getTotalPoints());
	totalPoints->setString(points);
}

void GameMainScene::createTimer()
{
	char time[100];
	sprintf(time, "%d", (int)DEFAULT_TIME_LEFT);
	Label* timer = Label::createWithSystemFont(time, "", 140);
	timer->setPosition(WIN_POS(0.5, 0.9));
	timer->setColor(Color3B(255, 0, 0));
	this->addChild(timer, static_cast<int>(ZOrder::Timer), static_cast<int>(Tag::Timer));
}

void GameMainScene::createBG()
{
	Sprite* bg = Sprite::createWithSpriteFrameName("BG.png");
	bg->setPosition(WIN_POS(0.5f, 0.5f));
	this->addChild(bg, static_cast<int>(ZOrder::BG), static_cast<int>(Tag::BG));

	b2BodyDef bgDef;
	bgDef.type = b2_staticBody;
	bgDef.position.Set(bg->getPosition().x / PTM_RATIO, bg->getPosition().y / PTM_RATIO);
	bgDef.userData = bg;
	b2Body* bgBody = _world->CreateBody(&bgDef);

	gbox2d::GB2ShapeCache* sc = gbox2d::GB2ShapeCache::getInstance();
	sc->addFixturesToBody(bgBody, "BG");
	bg->setAnchorPoint(sc->anchorPointForShape("BG"));
}

void GameMainScene::createDust()
{
	Sprite* dust = Sprite::createWithSpriteFrameName("dust_normal.png");
	dust->setPosition(WIN_POS(0.5, 0.5));
	this->addChild(dust, static_cast<int>(ZOrder::Dust), static_cast<int>(Tag::Dust));
}

void GameMainScene::createBallsInInit()
{
	for (std::shared_ptr<GameMainModel::BallInfo> ballInfo : _userController->getModel()->getBallsInfoList()) {
		createBall(ballInfo);
	}
}

void GameMainScene::createBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo)
{
	CustomPhysicsSprite* ball = CustomPhysicsSprite::createWithBallNumber(ballInfo->number);

	ball->setUp(_world, ballInfo);
	_ballsList.pushBack(ball);

	this->addChild(ball, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));
}

void GameMainScene::changeTextureOfBall(const std::shared_ptr<GameMainModel::BallInfo> ballInfo, const CustomPhysicsSprite::BallTextureType ballTextureType)
{
	CustomPhysicsSprite* ball = _ballsList.at(ballInfo->id);

	if (ball->getBallTextureType() == ballTextureType)
		return;

	ball->setNumber(ballInfo->number);
	ball->changeTexture(ballTextureType);

// アニメーション：ボール選択時
	if (ballTextureType == CustomPhysicsSprite::BallTextureType::Selected)
	{
		// ballをremoveすると、物理制御が解除されてしまう！
		this->reorderChild(ball, static_cast<int>(ZOrder::Ball));
		ScaleTo* scaleUp = ScaleTo::create(0.08f, 1.5f);
		ScaleTo* scaleDown = ScaleTo::create(0.08f, 1.0f);
		Sequence* seq = Sequence::create(scaleUp, scaleDown, nullptr);
		ball->runAction(seq);
	}
	else if (ballTextureType == CustomPhysicsSprite::BallTextureType::Normal)
	{
		//ball->setScale(1);
	}
}

void GameMainScene::pushDestroyBallsList(const std::shared_ptr<GameMainModel::BallInfo> ballInfo)
{
	CustomPhysicsSprite* ball = _ballsList.at(ballInfo->id);
	
	if (!ball->isPresence())
		return;

	ball->setPresence(false);
	//_destroyBallsList.pushBack(ball);
	//ball->destroy(_world);
}

void GameMainScene::setDestroyBallsAnimation()
{
	std::vector<std::shared_ptr<GameMainModel::BallInfo>> vanishAnimationBallsInfo = _userController->getModel()->getVanishAnimationBallsInfoList();

	int vanishAnimationBallsNumber = vanishAnimationBallsInfo.size();

	if (vanishAnimationBallsNumber == 0)
		return;
	
	else if (vanishAnimationBallsNumber == 1)
	{
		CustomPhysicsSprite* ball = _ballsList.at(vanishAnimationBallsInfo.at(0)->id);
		Sprite* blurBall = Sprite::createWithSpriteFrame(ball->getSpriteFrame());
		blurBall->setPosition(ball->getPosition());
		// 符号が逆になるので注意！
		blurBall->setRotation(-ball->getRotation());

		this->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

		CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameMainScene::destroyBall, this, ball));
		MoveTo* moveToCenter = MoveTo::create(0.1, WIN_POS(0.5, 0.5));
		ScaleTo* scaleDown = ScaleTo::create(0.2, 0.3);
		FadeTo* fadeOut = FadeTo::create(0.2, 0.0);
		Spawn* spawn = Spawn::create(scaleDown, fadeOut, nullptr);
		Sequence* seq = Sequence::create(func, moveToCenter, spawn, nullptr);
		blurBall->runAction(seq);
	}
	else
	{
		float t = 0.1f;

		for (int i = 0; i < vanishAnimationBallsNumber; ++i)
		{
			if (i == vanishAnimationBallsNumber - 1)
			{
				CustomPhysicsSprite* ball = _ballsList.at(vanishAnimationBallsInfo.at(i)->id);

				Sprite* blurBall = Sprite::createWithSpriteFrame(ball->getSpriteFrame());
				blurBall->setPosition(ball->getPosition());
				// 符号が逆になるので注意！
				blurBall->setRotation(-ball->getRotation());
				blurBall->setVisible(false);

				this->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

				DelayTime* delay = DelayTime::create(t * i * 0.8);
				Show* show = Show::create();
				CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameMainScene::destroyBall, this, ball));
				ScaleTo* scaleUp = ScaleTo::create(0.3, 1.5);
				FadeTo* fadeOut = FadeTo::create(0.3, 0.0);
				Spawn* spawn = Spawn::create(scaleUp, fadeOut, nullptr);
				RemoveSelf* removeSelf = RemoveSelf::create();
				Sequence* seq;
				if (vanishAnimationBallsInfo.at(i)->isPresence)
					seq = Sequence::create(delay, show, spawn, removeSelf, nullptr);
				else
					seq = Sequence::create(delay, show, func, spawn, removeSelf, nullptr);


				blurBall->runAction(seq);
			}
			else
			{
				CustomPhysicsSprite* ball = _ballsList.at(vanishAnimationBallsInfo.at(i)->id);
				CustomPhysicsSprite* nextBall = _ballsList.at(vanishAnimationBallsInfo.at(i+1)->id);

				Sprite* blurBall = Sprite::createWithSpriteFrame(ball->getSpriteFrame());
				blurBall->setPosition(ball->getPosition());
				// 符号が逆になるので注意！
				blurBall->setRotation(-ball->getRotation());
				blurBall->setVisible(false);

				this->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

				DelayTime* delay = DelayTime::create(t * i * 0.8);
				Show* show = Show::create();
				CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameMainScene::destroyBall, this, ball));
				MoveTo* moveToNextBall = MoveTo::create(t, nextBall->getPosition());
				FadeTo* fadeOut = FadeTo::create(t, 0.0);
				Spawn* spawn = Spawn::create(moveToNextBall, fadeOut, nullptr);
				RemoveSelf* removeSelf = RemoveSelf::create();
				Sequence* seq = Sequence::create(delay, show, func, spawn, removeSelf, nullptr);
				blurBall->runAction(seq);
			}

		}

	}
	_userController->setCompleteVanishAnimation();
}

void GameMainScene::destroyBall(CustomPhysicsSprite* ball)
{
	ball->destroy(_world);
}

void GameMainScene::createReturnButton()
{
	Sprite* btn = Sprite::createWithSpriteFrameName("HelloWorld.png");
	btn->setScale(0.5f);
	btn->setPosition(WIN_POS(0.1f, 0.1f));
	this->addChild(btn, 20);

	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [this](Touch* touch, Event* event)
	{
		Node* target = event->getCurrentTarget();
		Vec2 touchLocation = target->convertToNodeSpace(touch->getLocation());
		Rect targetRect = Rect(0, 0, target->getContentSize().width, target->getContentSize().height);

		if (targetRect.containsPoint(touchLocation)) {
			gotoNextScene(GameTitleScene::createScene());

//			if (_pauseFlag)
//			{
//				CustomPhysicsSprite::GM = GM_APPLY_FORCE;
//				_pauseFlag = false;
//			}
//			else
//			{
//				CustomPhysicsSprite::GM = 0.0f;
//				_pauseFlag = true;
//			}
		}

		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, btn);
}

Vector<CustomPhysicsSprite*> GameMainScene::getBallsList() const
{
	return _ballsList;
}

Sprite* GameMainScene::getDust() const
{
	return static_cast<Sprite*>(this->getChildByTag(static_cast<int>(Tag::Dust)));
}












