//
//  CustomPhysicsSprite.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/04.
//
//

#include "GameConfig.h"
#include "CustomPhysicsSprite.h"
#include "physicsEditor/GB2ShapeCache-x.h"
#include <math.h>

CustomPhysicsSprite* CustomPhysicsSprite::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    CustomPhysicsSprite* pRet = new CustomPhysicsSprite();
    if (pRet && pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}


void CustomPhysicsSprite::setUp(b2World* world, Vec2 position)
{
	b2BodyDef myDef;
	myDef.type = b2_dynamicBody;
	myDef.position.Set(position.x / PTM_RATIO, position.y / PTM_RATIO);
	myDef.userData = this;

	b2Body* myBody = world->CreateBody(&myDef);

	gbox2d::GB2ShapeCache* sc = gbox2d::GB2ShapeCache::getInstance();
	sc->addFixturesToBody(myBody, "ball");
	this->setAnchorPoint(sc->anchorPointForShape("ball"));

	this->setB2Body(myBody);
	this->setPTMRatio(PTM_RATIO);
	this->setPosition(position);

	this->scheduleUpdate();
}

void CustomPhysicsSprite::update(float dt)
{
	b2Body* body = this->getB2Body();

	float thisX = this->getPosition().x / PTM_RATIO;
	float thisY = this->getPosition().y / PTM_RATIO;
	float centerX = Director::getInstance()->getWinSize().width / 2 / PTM_RATIO;
	float centerY = Director::getInstance()->getWinSize().height / 2 / PTM_RATIO;

	float GM = 0.05f;

	float R2 = powf(thisX-centerX, 2.0f) + powf(thisY-centerY, 2.0f);
	float g = GM / R2;

	float R = sqrtf(R2);

	float gX = g * (centerX - thisX) / R;
	float gY = g * (centerY - thisY) / R;

	body->ApplyForce(b2Vec2(body->GetMass()*gX, body->GetMass()*gY), body->GetPosition(), false);
}




// 画面外に設置された場合、テクスチャがレンダリングされない（Sprite::draw）ので、
// ここでdrawをオーバライドして、Sprite::drawに処理がわたらないようにする。
void CustomPhysicsSprite::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	if (isDirty())
    {
        syncPhysicsTransform();
    }
	_quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &_quad, 1, _transform);
	renderer->addCommand(&_quadCommand);
}
