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
#include "CustomPhysicsSprite.h"


GameMainScene::~GameMainScene()
{
	for (b2Body* body = world->GetBodyList(); body; body = body->GetNext()) {
		world->DestroyBody(body);
	}
	delete world;
	world = nullptr;
	
	dispose("GameMainScene");
}

GameMainScene::GameMainScene()
{
	userController = std::make_shared<GameMainUserController>();
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
		
		Texture2D* texture = Director::getInstance()->getTextureCache()->getTextureForKey(IMAGE_NAME(imageName));
		gameMainBatchNode =SpriteBatchNode::createWithTexture(texture);
		this->addChild(gameMainBatchNode);

		initPhysics();
		createBG();
		createReturnButton();

		for (int i = 0; i < 60; ++i) {
			createBall();
		}
	}

	pleaseWaitLayer_out(CC_CALLBACK_0(GameMainScene::startGame, this));
}

void GameMainScene::initPhysics()
{
	gbox2d::GB2ShapeCache::getInstance()->addShapesWithFile("Physics/GameMainScenePhysics.plist");

	b2Vec2 gravity;
	gravity.Set(0.0f, 0.0f);
	world = new b2World(gravity);
}

void GameMainScene::startGame()
{
	this->scheduleUpdate();

	setEventListener();
}

void GameMainScene::update(float dt)
{
	world->Step(APP_FPS, VELOCITY_ITERATION, POSITION_ITERATION);
}

void GameMainScene::setEventListener()
{
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(GameMainUserController::touchBegan, userController, this);
	listener->onTouchMoved = CC_CALLBACK_2(GameMainUserController::touchMoved, userController, this);
	listener->onTouchEnded = CC_CALLBACK_2(GameMainUserController::touchEnded, userController, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, gameMainBatchNode);
}

void GameMainScene::createBG()
{
	Sprite* bg = Sprite::createWithSpriteFrameName("BG.png");
	bg->setPosition(WIN_POS(0.5f, 0.5f));
	gameMainBatchNode->addChild(bg);

	b2BodyDef bgDef;
	bgDef.type = b2_staticBody;
	bgDef.position.Set(bg->getPosition().x / PTM_RATIO, bg->getPosition().y / PTM_RATIO);
	bgDef.userData = bg;
	b2Body* bgBody = world->CreateBody(&bgDef);

	gbox2d::GB2ShapeCache* sc = gbox2d::GB2ShapeCache::getInstance();
	sc->addFixturesToBody(bgBody, "BG");
	bg->setAnchorPoint(sc->anchorPointForShape("BG"));
}

void GameMainScene::createBall()
{
	CustomPhysicsSprite* ball = CustomPhysicsSprite::createWithSpriteFrameName("ball1.png");

	Vec2 position = WIN_POS(1.2, 0.5);

	ball->setUp(world, position);
	gameMainBatchNode->addChild(ball);
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
		}

		return false;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, btn);
}














