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
#include "cocostudio/CocoStudio.h"


GameMainScene::~GameMainScene()
{
	b2Body* node = _world->GetBodyList();
	while (node)
	{
		b2Body* b = node;
		node = node->GetNext();
		_world->DestroyBody(b);
	}

	delete _world;
	_world = nullptr;
	
	dispose(textureAtlasName);

	_ballsList.clear();
	_destroyBallsList.clear();

	gbox2d::GB2ShapeCache::getInstance()->reset();
}

GameMainScene::GameMainScene()
{
	textureAtlasName = "GameMainScene";
	
	_userController = std::make_shared<GameMainUserController>();

	_dustSelectedFlag = false;
	_pauseFlag = false;
	_timeupFlag = false;
	_last10Flag = false;
	_bonusModeFlag = false;
	_preAddingPoints = 0;
	_preTotalPoints = _userController->getModel()->getTotalPoints();
	_realyTotalPoints = _preTotalPoints;
	_preTimeLeft = _userController->getModel()->getTimeLeft();
	CustomPhysicsSprite::GM = 0.0f;
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
	loadTextureAtlasAsync(textureAtlasName);
}

void GameMainScene::sceneSetting(const std::string imageName)
{
	if (imageName == textureAtlasName) {

		initPhysics();
		createUI();
		createBallsInInit();
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
	CustomPhysicsSprite::GM = GM_APPLY_FORCE;
	createCountDown();
	this->scheduleUpdate();
	this->schedule(schedule_selector(GameMainScene::updateTotalPoints));
}

void GameMainScene::createCountDown()
{
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ready")->setVisible(true);
	cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "ready");

}

void GameMainScene::openCountDown()
{
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_onReady_blackFilter")->setVisible(false);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_go")->setVisible(true);

	CallFunc* func = CallFunc::create([this](){
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ready")->setVisible(false);
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_go")->setVisible(false);
	});

	cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "go", func);

	setEventListener();
}

void GameMainScene::update(float dt)
{
	if (_pauseFlag)
		return;

	_world->Step(APP_FPS, VELOCITY_ITERATION, POSITION_ITERATION);
	
	_userController->sendDtAndBalls(dt, _ballsList);

	checkTime();
	checkBalls();
	checkDust();
	checkPoints();
	checkChainPoints();
}

void GameMainScene::setEventListener()
{
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameMainUserController::touchBegan, _userController, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameMainUserController::touchMoved, _userController, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameMainUserController::touchEnded, _userController, this);

	// 空のレイヤを無闇に増やすのもアレなんで、とりあえずエフェクトコンテナにイベントを付けておく。
	ui::Layout* touchEventLayer = dynamic_cast<ui::Layout*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer"));
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, touchEventLayer);
}

void GameMainScene::checkTime()
{
	if (_timeupFlag)
		return;

	if (_userController->getModel()->getGameStatus() == GameMainModel::GameStatus::Playing)
	{
		if (_userController->getModel()->getGamePrevStatus() == GameMainModel::GameStatus::Ready)
		{
			openCountDown();
		}
	}
	
	if (_userController->getModel()->getGameStatus() == GameMainModel::GameStatus::TimeUp)
	{
		if(_userController->getModel()->getGamePrevStatus() == GameMainModel::GameStatus::Playing)
		{
			ui::Layout* touchEventLayer = dynamic_cast<ui::Layout*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer"));

			Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(touchEventLayer);

			this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_all_blackFilter")->setOpacity(255);
			this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_all_blackFilter")->setVisible(true);
			this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timeup")->setVisible(true);

			ProgressTimer* progressTimerBG = dynamic_cast<ProgressTimer*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timerBGContainer")->getChildByTag(static_cast<int>(Tag::TimerBG)));
			progressTimerBG->setPercentage(100.0f);

			ui::Button* pauseButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseButton"));
			pauseButton->setBright(false);
			pauseButton->setTouchEnabled(false);

			cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "timeup");

			if (_bonusModeFlag)
				setBonusModeOut();

			this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->getChildByName("GameMainScene_drawNodeContainer")->removeAllChildrenWithCleanup(true);

			_timeupFlag = true;
			return;
		}
	}

	float tl = _userController->getModel()->getTimeLeft();
	int timeLeft = static_cast<int>(floorf(tl));
	float decimal = tl - (float)timeLeft;

	if (timeLeft < 6)
	{
		ProgressTimer* progressTimerBG = dynamic_cast<ProgressTimer*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timerBGContainer")->getChildByTag(static_cast<int>(Tag::TimerBG)));

		progressTimerBG->setPercentage(decimal * 100.0f);

	}

	if (timeLeft == _preTimeLeft)
		return;

	_preTimeLeft = timeLeft;
	
	char time[100];

	ui::Text* timer = dynamic_cast<ui::Text*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timer"));

	sprintf(time, "%d", timeLeft);
	timer->setString(time);

	timer->setScale(1.3);
	timer->setOpacity(0);

	ScaleTo* scaleUp = ScaleTo::create(0.3f, 1.0f);
	FadeTo* fadeIn = FadeTo::create(0.3f, 255);
	Spawn* spawn = Spawn::create(scaleUp, fadeIn, nullptr);
	EaseCubicActionOut* ease = EaseCubicActionOut::create(spawn);

	timer->runAction(ease);


	if (timeLeft < 11 && !_last10Flag)
	{
		Sprite* timerBG = Sprite::createWithSpriteFrameName("GameMainScene_timerBG_red.png");
		ProgressTimer* progressTimerBG = dynamic_cast<ProgressTimer*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timerBGContainer")->getChildByTag(static_cast<int>(Tag::TimerBG)));
		progressTimerBG->setSprite(timerBG);
		progressTimerBG->setPercentage(99.99999f); // こうしないとなぜか表示されない。
		_last10Flag = true;
	}
}

void GameMainScene::checkBalls()
{
	if	(_userController->getModel()->getBallsInfoList().size() > _ballsList.size())
	{
		int addBallsNumber = static_cast<int>(_userController->getModel()->getBallsInfoList().size()) - static_cast<int>(_ballsList.size());
		int size = static_cast<int>(_ballsList.size());
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
		if (!ballInfo->isSelectEnable)
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Disabled);
		}
		else if (ballInfo->isSelected)
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Selected);
		}
		else
		{
			changeTextureOfBall(ballInfo, CustomPhysicsSprite::BallTextureType::Normal);
		}
	}

	drawLineOnSelectedBalls();

	setDestroyBallsAnimation();
}

void GameMainScene::drawLineOnSelectedBalls()
{
	if (_timeupFlag)
		return;

	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->getChildByName("GameMainScene_drawNodeContainer")->removeAllChildrenWithCleanup(true);

	std::vector<std::shared_ptr<GameMainModel::BallInfo>> selectedBallsList = _userController->getModel()->getSelectedBallsInfoList();
	int size = selectedBallsList.size();

	if (size < 2)
		return;

	for (int i = 0; i < size-1; ++i)
	{
		DrawNode* drawNode1 = DrawNode::create();
		drawNode1->drawSegment(Vec2(selectedBallsList.at(i)->x, selectedBallsList.at(i)->y), Vec2(selectedBallsList.at(i+1)->x, selectedBallsList.at(i+1)->y), 6.0f, Color4F(1.0f, 0.0f, 0.5f, 0.7f));
		drawNode1->setBlendFunc(BlendFunc::ADDITIVE);

		DrawNode* drawNode2 = DrawNode::create();
		drawNode2->drawSegment(Vec2(selectedBallsList.at(i)->x, selectedBallsList.at(i)->y), Vec2(selectedBallsList.at(i+1)->x, selectedBallsList.at(i+1)->y), 3.0f, Color4F(1.0f, 1.0f, 1.0f, 0.7f));
		drawNode2->setBlendFunc(BlendFunc::ADDITIVE);


		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->getChildByName("GameMainScene_drawNodeContainer")->addChild(drawNode1);

		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->getChildByName("GameMainScene_drawNodeContainer")->addChild(drawNode2);
	}
}

void GameMainScene::checkDust()
{
	if (_dustSelectedFlag == _userController->getModel()->isDustSelected())
		return;

	_dustSelectedFlag = !_dustSelectedFlag;

	if (_dustSelectedFlag)
	{
		this->getDust()->loadTexture("GameMainScene_dust_selected.png", TextureResType::PLIST);
		animationDustBlur();
	}
	else
		this->getDust()->loadTexture("GameMainScene_dust_normal.png", TextureResType::PLIST);
}

void GameMainScene::animationDustBlur()
{
	Sprite* dustBlur = dynamic_cast<Sprite*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->getChildByTag(static_cast<int>(Tag::DustBlur)));

	dustBlur->stopAllActions();
	dustBlur->setScale(1.0f);
	dustBlur->setOpacity(255);

	Show* show = Show::create();
	ScaleTo* scaleUp = ScaleTo::create(0.5f, 1.3f);
	FadeTo* fadeOut = FadeTo::create(0.5f, 0);
	Spawn* spawn = Spawn::create(scaleUp, fadeOut, nullptr);
	EaseCubicActionOut* ease = EaseCubicActionOut::create(spawn);
	Hide* hide = Hide::create();

	Sequence* seq = Sequence::create(show, ease, hide, nullptr);

	dustBlur->runAction(seq);
}

void GameMainScene::checkPoints()
{
	char points[100];

	ui::Text* addingPoints = dynamic_cast<ui::Text*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_addingPoints"));

	int ap = _userController->getModel()->getAddingPoints();

	int tp = _userController->getModel()->getTotalPoints();


	if (tp > _preTotalPoints)
	{
		_preTotalPoints = tp;

		// トータルポイントのカウンタが回るのを遅らせる
		DelayTime* delay = DelayTime::create(0.7f);
		CallFunc* func = CallFunc::create(CC_CALLBACK_0(GameMainScene::DelaySetTotalPoints, this, tp));
		Sequence* seq = Sequence::create(delay, func, nullptr);
		this->runAction(seq);

		// ブラーアディングポイントの生成
		{
			std::string apb;
			if (_bonusModeFlag)
				apb = "+" + std::to_string(_preAddingPoints * BONUS_MODE_POINTS_SCALE_FACTOR);
			else
				apb = "+" + std::to_string(_preAddingPoints);

			ui::Text* addingPointsBlur = ui::Text::create(apb, addingPoints->getFontName(), addingPoints->getFontSize());
			this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->addChild(addingPointsBlur);
			addingPointsBlur->setPosition(Vec2(addingPoints->getPosition().x-10, addingPoints->getPosition().y));
			addingPointsBlur->setScale(1.0f);
			addingPointsBlur->setOpacity(255);
			if (_preAddingPoints > 10)
				addingPointsBlur->setColor(CUSTOM_COLOR_YELLOW);
			else
				addingPointsBlur->setColor(CUSTOM_COLOR_GREEN);

			ScaleTo* scaleUp = ScaleTo::create(0.2f, 1.2f);
			FadeTo* fadeIn = FadeTo::create(0.2f, 255);
			Spawn* spawn1 = Spawn::create(scaleUp, fadeIn, nullptr);
			EaseCubicActionOut* ease1 = EaseCubicActionOut::create(spawn1);

			MoveBy* moveH = MoveBy::create(0.5f, Vec2(WIN_POS(-0.23f, 0.0f)));
			EaseExponentialOut* ease2 = EaseExponentialOut::create(moveH);

			MoveBy* moveV = MoveBy::create(0.1f, Vec2(WIN_POS(0.0f, 0.05f)));
			FadeTo* fadeOut = FadeTo::create(0.1f, 0);
			Spawn* spawn2 = Spawn::create(moveV, fadeOut, nullptr);
			EaseCubicActionIn* ease3 = EaseCubicActionIn::create(spawn2);

			RemoveSelf* remove = RemoveSelf::create();

			Sequence* seq2 = Sequence::create(ease1, ease2, ease3, remove, nullptr);
			addingPointsBlur->runAction(seq2);


			addingPoints->setOpacity(32);
			FadeTo* fadeReturn = FadeTo::create(0.3f, 255);
			EaseExponentialIn* easeReturn = EaseExponentialIn::create(fadeReturn);
			addingPoints->runAction(easeReturn);
		}

		// トータルポイントBGのアニメーション
		{
			ui::ImageView* pointsBG = dynamic_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_header")->getChildByName("GameMainScene_header_pointsBG"));
			pointsBG->stopAllActions();
			pointsBG->setOpacity(255);
			pointsBG->setVisible(true);
			if (_preAddingPoints > 10)
				pointsBG->loadTexture("GameMainScene_header_pointsBG_yellow.png", TextureResType::PLIST);
			else
				pointsBG->loadTexture("GameMainScene_header_pointsBG_green.png", TextureResType::PLIST);
			Blink* blink = Blink::create(1.2f, 5);
			Hide* hide = Hide::create();
			Sequence* seq = Sequence::create(blink, hide, nullptr);
			pointsBG->runAction(seq);
		}

	}

	if (ap != _preAddingPoints)
	{
		_preAddingPoints = ap;

		sprintf(points, "%d", ap);
		addingPoints->setString(points);

		if (ap > 10)
			addingPoints->setColor(CUSTOM_COLOR_YELLOW);
		else
			addingPoints->setColor(CUSTOM_COLOR_BLUE);
	}
}

void GameMainScene::DelaySetTotalPoints(int preTotalPoints)
{
	_realyTotalPoints = preTotalPoints;
}

void GameMainScene::updateTotalPoints(float dt)
{
	ui::Text* totalPoints = dynamic_cast<ui::Text*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_totalPoints"));

	int currentTotalPoint = stoi(totalPoints->getString());
	if (_realyTotalPoints > currentTotalPoint)
	{
		++currentTotalPoint;
		totalPoints->setString(std::to_string(currentTotalPoint));
	}
}

void GameMainScene::checkChainPoints()
{
	if (_userController->getModel()->getGamePrevStatus() == GameMainModel::GameStatus::Playing)
	{
		if (_userController->getModel()->getGameStatus() == GameMainModel::GameStatus::BonusModeIn)
		{
			setBonusModeIn();
		}
		else if (_userController->getModel()->getGameStatus() == GameMainModel::GameStatus::BonusModeOut)
		{
			setBonusModeOut();
		}
	}

	setChainUI();
}

void GameMainScene::setBonusModeIn()
{
	_bonusModeFlag = true;
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_bonusAlert")->setVisible(true);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BG")->setColor(Color3B(128, 128, 128));
	CallFunc* func = CallFunc::create([this](){
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_bonusAlert")->setVisible(false);
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_header")->getChildByName("GameMainScene_header_bonusParts")->setVisible(true);
	});
	cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "bonusAlert", func);

	{
		Sprite* pentagon1 = static_cast<Sprite*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByTag(static_cast<int>(Tag::Pentagon1)));
		pentagon1->setVisible(true);
		Vector<FiniteTimeAction*> vec;
		for (int i = 0; i < 18; ++i) {
			vec.pushBack(ScaleBy::create(0.0f, (1.0f / 0.9f)));
			vec.pushBack(DelayTime::create(0.07f));
		}
		vec.pushBack(ScaleTo::create(0.0f, powf(0.9f, 6.0f)));
		Sequence* seq = Sequence::create(vec);
		RepeatForever* repeat = RepeatForever::create(seq);
		pentagon1->runAction(repeat);
	}
	{
		Sprite* pentagon2 = static_cast<Sprite*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByTag(static_cast<int>(Tag::Pentagon2)));
		CallFunc* func = CallFunc::create([pentagon2](){
			pentagon2->setVisible(true);
			Vector<FiniteTimeAction*> vec;
			for (int i = 0; i < 18; ++i) {
				vec.pushBack(ScaleBy::create(0.0f, (1.0f / 0.9f)));
				vec.pushBack(DelayTime::create(0.07f));
			}
			vec.pushBack(ScaleTo::create(0.0f, powf(0.9f, 6.0f)));
			Sequence* seq = Sequence::create(vec);
			RepeatForever* repeat = RepeatForever::create(seq);
			pentagon2->runAction(repeat);
		});
		DelayTime* delay = DelayTime::create(0.07f * 9.0f);
		Sequence* seq = Sequence::create(delay, func, nullptr);
		pentagon2->runAction(seq);
	}
	{
		ui::ImageView* chainUILight = static_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_footer")->getChildByName("GameMainScene_chainUI_light"));
		chainUILight->setVisible(true);
		chainUILight->setOpacity(128);
		FadeTo* fadeUp = FadeTo::create(0.5f, 255);
		EaseCubicActionInOut* easeUp = EaseCubicActionInOut::create(fadeUp);
		FadeTo* fadeDown = FadeTo::create(0.5f, 128);
		EaseCubicActionInOut* easeDown = EaseCubicActionInOut::create(fadeDown);
		Sequence* seq = Sequence::createWithTwoActions(easeUp, easeDown);
		RepeatForever* repeat = RepeatForever::create(seq);
		chainUILight->runAction(repeat);
	}

	{
		ui::LoadingBar* chainPtMeter = static_cast<ui::LoadingBar*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_chainPtMeter"));
		chainPtMeter->loadTexture("GameMainScene_chainPtMeter_in.png", TextureResType::PLIST);
	}

	{
		Vector<Node*> vec = this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByName("GameMainScene_radialRaysContainer")->getChildren();
		int size = vec.size();
		for (int i = 0; i < size; ++i) {
			Sprite* radialRay = dynamic_cast<Sprite*>(vec.at(i));

			float startDegree = CCRANDOM_0_1() * 360.0f;
			float deltaDegree = CCRANDOM_MINUS1_1() * 90.0f + 45.0f;
			radialRay->setScaleY(135.0f / std::abs(deltaDegree));
			radialRay->setRotation(startDegree);
			radialRay->setOpacity(255);

			DelayTime* delay = DelayTime::create((float)i * 0.05);
			Show* show = Show::create();
			RotateBy* rotate = RotateBy::create(3.0f, deltaDegree);
			FadeTo* fadeOut = FadeTo::create(3.0f, 0);
			Spawn* spawn = Spawn::create(rotate, fadeOut, NULL);
			EaseCubicActionOut* ease = EaseCubicActionOut::create(spawn);
			Hide* hide = Hide::create();
			Sequence* seq = Sequence::create(delay, show, ease, hide, nullptr);

			radialRay->runAction(seq);
		}
	}
}

void GameMainScene::setBonusModeOut()
{
	_bonusModeFlag = false;
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_header")->getChildByName("GameMainScene_header_bonusParts")->setVisible(false);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BG")->setColor(Color3B(255, 255, 255));

	Sprite* pentagon1 = static_cast<Sprite*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByTag(static_cast<int>(Tag::Pentagon1)));
	pentagon1->setVisible(false);
	pentagon1->setScale(powf(0.9f, 6.0f));
	pentagon1->stopAllActions();

	Sprite* pentagon2 = static_cast<Sprite*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByTag(static_cast<int>(Tag::Pentagon2)));
	pentagon2->setVisible(false);
	pentagon2->setScale(powf(0.9f, 6.0f));
	pentagon2->stopAllActions();

	ui::ImageView* chainUILight = static_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_footer")->getChildByName("GameMainScene_chainUI_light"));
	chainUILight->setVisible(false);
	chainUILight->stopAllActions();

	ui::LoadingBar* chainPtMeter = static_cast<ui::LoadingBar*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_chainPtMeter"));
	chainPtMeter->loadTexture("GameMainScene_chainPtMeter_out.png", TextureResType::PLIST);
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

	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ballsContainer")->addChild(ball, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));
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
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ballsContainer")->reorderChild(ball, static_cast<int>(ZOrder::Ball));
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

	int vanishAnimationBallsNumber = static_cast<int>(vanishAnimationBallsInfo.size());

	if (vanishAnimationBallsNumber == 0)
		return;
	
	else if (vanishAnimationBallsNumber == 1)
	{
		CustomPhysicsSprite* ball = _ballsList.at(vanishAnimationBallsInfo.at(0)->id);
		Sprite* blurBall = Sprite::createWithSpriteFrame(ball->getSpriteFrame());
		blurBall->setPosition(ball->getPosition());
		// 符号が逆になるので注意！
		blurBall->setRotation(-ball->getRotation());

		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ballsContainer")->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

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

				this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ballsContainer")->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

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

				this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ballsContainer")->addChild(blurBall, static_cast<int>(ZOrder::Ball), static_cast<int>(Tag::Ball));

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
}

void GameMainScene::destroyBall(CustomPhysicsSprite* ball)
{
	ball->destroy(_world);
}

void GameMainScene::createUI()
{
	this->addChild( cocostudio::GUIReader::getInstance()->widgetFromJsonFile("GameMainScene.ExportJson"), static_cast<int>(ZOrder::UI) );

	ui::ImageView* bg = dynamic_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BG"));

	b2BodyDef bgDef;
	bgDef.type = b2_staticBody;
	bgDef.position.Set(bg->getPosition().x / PTM_RATIO, bg->getPosition().y / PTM_RATIO);
	bgDef.userData = bg;
	b2Body* bgBody = _world->CreateBody(&bgDef);

	gbox2d::GB2ShapeCache* sc = gbox2d::GB2ShapeCache::getInstance();
	sc->addFixturesToBody(bgBody, "GameMainScene_BG");
	bg->setAnchorPoint(sc->anchorPointForShape("GameMainScene_BG"));


	// タイマーを初期化
	ui::Text* timer = dynamic_cast<ui::Text*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timer"));
	timer->setString(std::to_string(_preTimeLeft));

	// ダストブラーを初期化
	Sprite* dustBlur = Sprite::createWithSpriteFrameName("GameMainScene_dust_blur.png");
	dustBlur->setVisible(false);
	dustBlur->setPosition(WIN_POS(0.5f, 0.5f));
	dustBlur->setTag(static_cast<int>(Tag::DustBlur));
	dustBlur->setBlendFunc(BlendFunc::ADDITIVE);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer")->addChild(dustBlur);

	// タイマーBGを初期化
	Sprite* timerBG = Sprite::createWithSpriteFrameName("GameMainScene_timerBG_blue.png");
	ProgressTimer* progressTimerBG = ProgressTimer::create(timerBG);
	progressTimerBG->setType(ProgressTimer::Type::RADIAL);
	progressTimerBG->setTag(static_cast<int>(Tag::TimerBG));
	progressTimerBG->setPercentage(100.0f);
	progressTimerBG->setReverseDirection(true);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_timerBGContainer")->addChild(progressTimerBG);

	// チェインポイントを初期化
	setChainUI();

	Sprite* pentagon1 = Sprite::createWithSpriteFrameName("GameMainScene_pentagonLine.png");
	pentagon1->setTag(static_cast<int>(Tag::Pentagon1));
	pentagon1->setVisible(false);
	pentagon1->setBlendFunc(BlendFunc::ADDITIVE);
	pentagon1->setScale(powf(0.9f, 6.0f));
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->addChild(pentagon1);

	Sprite* pentagon2 = Sprite::createWithSpriteFrameName("GameMainScene_pentagonLine.png");
	pentagon2->setTag(static_cast<int>(Tag::Pentagon2));
	pentagon2->setVisible(false);
	pentagon2->setBlendFunc(BlendFunc::ADDITIVE);
	pentagon2->setScale(powf(0.9f, 6.0f));
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->addChild(pentagon2);

	for (int i = 0; i < 20; ++i) {
		Sprite* radialRay = Sprite::createWithSpriteFrameName("GameMainScene_radialRay.png");
		radialRay->setAnchorPoint(Vec2(0.0f, 0.5f));
		radialRay->setBlendFunc(BlendFunc::ADDITIVE);
		radialRay->setVisible(false);
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_BGEffectsContainer")->getChildByName("GameMainScene_radialRaysContainer")->addChild(radialRay);
	}

	createPauseButton();
	createContinueButton();
	createScoreButton();
	createRetryButton();

	// レイアウトのデフォルトのタッチイベントを解除、さもないとポーズボタンが押せない＆レイアウトに新たなタッチイベントが付けられない
	dynamic_cast<ui::Layout*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_effectsContainer"))->setEnabled(false);

	// エフェクトコンテナの上にかぶさるものは、タッチ無効にしておく
	dynamic_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_go"))->setEnabled(false);
	dynamic_cast<ui::Layout*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_ready"))->setEnabled(false);
	dynamic_cast<ui::Layout*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_bonusAlert"))->setEnabled(false);
}

void GameMainScene::setChainUI()
{
	ui::LoadingBar* chainPtMeter = dynamic_cast<ui::LoadingBar*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_chainPtMeter"));

	ui::Text* chainPoints = dynamic_cast<ui::Text*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_chainPoints"));

	float pt = _userController->getModel()->getChainPoints();
	chainPtMeter->setPercent(_userController->getModel()->getChainPoints());

	char st[10];
	sprintf(st, "%d %%", static_cast<int>(floorf(pt)));
	chainPoints->setString(st);
}

void GameMainScene::createPauseButton()
{
	ui::Button* pauseButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseButton"));

	pauseButton->setPressedActionEnabled(false);
	pauseButton->addTouchEventListener(CC_CALLBACK_2(GameMainScene::touchEvent_pauseButton, this));
}

void GameMainScene::touchEvent_pauseButton(Ref* pSender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;

        case ui::Widget::TouchEventType::MOVED:
            break;

        case ui::Widget::TouchEventType::ENDED:
			dynamic_cast<ui::Button*>(pSender)->setTouchEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setHighlighted(true);
			gamePause();
            break;

        case ui::Widget::TouchEventType::CANCELED:
            break;

        default:
            break;
    }
}

void GameMainScene::createContinueButton()
{
	ui::Button* continueButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->getChildByName("GameMainScene_onPause_parts1")->getChildByName("GameMainScene_onPause_continueButton"));

	continueButton->setPressedActionEnabled(true);
	continueButton->addTouchEventListener(CC_CALLBACK_2(GameMainScene::touchEvent_continueButton, this));
}

void GameMainScene::touchEvent_continueButton(Ref* pSender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;

        case ui::Widget::TouchEventType::MOVED:
            break;

        case ui::Widget::TouchEventType::ENDED:
			dynamic_cast<ui::Button*>(pSender)->setPressedActionEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setTouchEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setHighlighted(true);
			gameContinue();
			break;

        case ui::Widget::TouchEventType::CANCELED:
            break;

        default:
            break;
    }
}

void GameMainScene::createScoreButton()
{
	ui::Button* scoreButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->getChildByName("GameMainScene_onPause_parts1")->getChildByName("GameMainScene_onPause_scoreButton"));

	scoreButton->setPressedActionEnabled(true);
	scoreButton->addTouchEventListener(CC_CALLBACK_2(GameMainScene::touchEvent_scoreButton, this));
}

void GameMainScene::touchEvent_scoreButton(Ref* pSender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;

        case ui::Widget::TouchEventType::MOVED:
            break;

        case ui::Widget::TouchEventType::ENDED:
			dynamic_cast<ui::Button*>(pSender)->setPressedActionEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setTouchEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setHighlighted(true);
			gotoNextScene(GameTitleScene::createScene());
            break;

        case ui::Widget::TouchEventType::CANCELED:
            break;

        default:
            break;
    }
}

void GameMainScene::createRetryButton()
{
	ui::Button* retryButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->getChildByName("GameMainScene_onPause_parts1")->getChildByName("GameMainScene_onPause_retryButton"));

	retryButton->setPressedActionEnabled(true);
	retryButton->addTouchEventListener(CC_CALLBACK_2(GameMainScene::touchEvent_retryButton, this));
}

void GameMainScene::touchEvent_retryButton(Ref* pSender, ui::Widget::TouchEventType type)
{
    switch (type)
    {
        case ui::Widget::TouchEventType::BEGAN:
            break;

        case ui::Widget::TouchEventType::MOVED:
            break;

        case ui::Widget::TouchEventType::ENDED:
			dynamic_cast<ui::Button*>(pSender)->setPressedActionEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setTouchEnabled(false);
			dynamic_cast<ui::Button*>(pSender)->setHighlighted(true);
			gotoNextScene(GameMainScene::createScene());
            break;

        case ui::Widget::TouchEventType::CANCELED:
            break;

        default:
            break;
    }
}

void GameMainScene::gamePause()
{
	CustomPhysicsSprite::GM = 0.0f;
	_pauseFlag = true;

	ui::Button* continueButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->getChildByName("GameMainScene_onPause_parts1")->getChildByName("GameMainScene_onPause_continueButton"));
	continueButton->setPressedActionEnabled(true);
	continueButton->setTouchEnabled(true);
	continueButton->setHighlighted(false);

	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_all_blackFilter")->setVisible(true);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->setVisible(true);
	this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseBlinder")->setVisible(true);
	cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "pause_in");
}

void GameMainScene::gameContinue()
{
	CallFunc* func = CallFunc::create([this](){
		CustomPhysicsSprite::GM = GM_APPLY_FORCE;
		_pauseFlag = false;

		ui::Button* pauseButton = dynamic_cast<ui::Button*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseButton"));
		pauseButton->setTouchEnabled(true);
		pauseButton->setHighlighted(false);

		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_all_blackFilter")->setVisible(false);
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseLayer")->setVisible(false);
		this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_pauseBlinder")->setVisible(false);
	});

	cocostudio::ActionManagerEx::getInstance()->playActionByName("GameMainScene.ExportJson", "pause_out", func);
}



Vector<CustomPhysicsSprite*> GameMainScene::getBallsList() const
{
	return _ballsList;
}

ui::ImageView* GameMainScene::getDust() const
{
	return dynamic_cast<ui::ImageView*>(this->getChildByName("GameMainScene_UI")->getChildByName("GameMainScene_dust"));
}












