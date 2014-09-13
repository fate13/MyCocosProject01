//
//  CustomPhysicsSprite.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/04.
//
//

#include "AppMacros.h"
#include "GameConfig.h"
#include "CustomPhysicsSprite.h"
#include "physicsEditor/GB2ShapeCache-x.h"
#include <math.h>

float CustomPhysicsSprite::GM = GM_APPLY_FORCE;

CustomPhysicsSprite::CustomPhysicsSprite()
{
	_destroyFlag = false;
}

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

CustomPhysicsSprite* CustomPhysicsSprite::createWithBallNumber(const int ballNumber)
{
    char ballImageName[100];
	sprintf(ballImageName, "ball%d_normal.png", ballNumber);

	CustomPhysicsSprite* ball = CustomPhysicsSprite::createWithSpriteFrameName(ballImageName);

	return ball;
}

void CustomPhysicsSprite::setUp(b2World* world, const std::shared_ptr<GameMainModel::BallInfo> ballInfo)
{
	Vec2 position = WIN_POS(ballInfo->initX, ballInfo->initY);

//	b2BodyDef catBodyDef;
//	catBodyDef.type = b2_staticBody;
//	catBodyDef.position.Set(catPosition.x / PTM_RATIO, catPosition.y / PTM_RATIO);
//	catBodyDef.userData = pCat;
//	catBody = world->CreateBody(&catBodyDef);
//
//	b2CircleShape catShape;
//	catShape.m_radius = pCat->getContentSize().width * 0.4 / PTM_RATIO;
//
//	b2FixtureDef catFixturedef;
//	catFixturedef.shape = &catShape;
//	catFixturedef.density = 1;
//	catFixturedef.friction = 0.3;
//	catBody->CreateFixture(&catFixturedef);


	b2BodyDef myDef;
	myDef.type = b2_dynamicBody;
	myDef.position.Set(position.x / PTM_RATIO, position.y / PTM_RATIO);
	myDef.userData = this;

	b2Body* myBody = world->CreateBody(&myDef);

//	gbox2d::GB2ShapeCache* sc = gbox2d::GB2ShapeCache::getInstance();
//	sc->addFixturesToBody(myBody, "ball");
//	this->setAnchorPoint(sc->anchorPointForShape("ball"));

		b2CircleShape catShape;
		catShape.m_radius = this->getContentSize().width * 0.5 / PTM_RATIO;

		b2FixtureDef catFixturedef;
		catFixturedef.shape = &catShape;
		catFixturedef.density = 1;
		catFixturedef.friction = 0.3;
		myBody->CreateFixture(&catFixturedef);

	this->setB2Body(myBody);
	this->setPTMRatio(PTM_RATIO);
	this->setPosition(position);

	_id = ballInfo->id;
	_number = ballInfo->number;
	_isPresence = ballInfo->isPresence;
	_isSelectEnable = ballInfo->isSelectEnable;
	_isSelected = ballInfo->isSelected;
	_ballTextureType = BallTextureType::Normal;

	b2Body* body = this->getB2Body();
	body->ApplyLinearImpulse(b2Vec2(body->GetMass()*ballInfo->impulseX, body->GetMass()*ballInfo->impulseY), body->GetPosition(), false);

	this->scheduleUpdate();
}

void CustomPhysicsSprite::changeTexture(const BallTextureType ballTextureType)
{
	char ballImageName[100];

	switch (ballTextureType) {
		case BallTextureType::Normal:
			sprintf(ballImageName, "ball%d_normal.png", _number);
			this->setSpriteFrame(ballImageName);
			break;

		case BallTextureType::Selected:
			sprintf(ballImageName, "ball%d_selected.png", _number);
			this->setSpriteFrame(ballImageName);
			break;

		case BallTextureType::Disabled:
			sprintf(ballImageName, "ball%d_disabled.png", _number);
			this->setSpriteFrame(ballImageName);
			break;

		default:
			break;
	}
	_ballTextureType = ballTextureType;
}

void CustomPhysicsSprite::destroy(b2World* world)
{
	world->DestroyBody(this->getB2Body());
	this->setPresence(false);
	this->setVisible(false);
	this->removeFromParentAndCleanup(true);
	_destroyFlag = true;
}


int CustomPhysicsSprite::getId() const
{
	return _id;
}

void CustomPhysicsSprite::setNumber(const int number)
{
	_number = number;
}

int CustomPhysicsSprite::getNumber() const
{
	return _number;
}

void CustomPhysicsSprite::setPresence(const bool isPresence)
{
	_isPresence = isPresence;
}

bool CustomPhysicsSprite::isPresence() const
{
	return _isPresence;
}

void CustomPhysicsSprite::setSelectEnable(const bool isSelectEnable)
{
	_isSelectEnable = isSelectEnable;
}

bool CustomPhysicsSprite::isSelectEnable() const
{
	return _isSelectEnable;
}

void CustomPhysicsSprite::setSelected(const bool isSelected)
{
	_isSelected = isSelected;
}

bool CustomPhysicsSprite::isSelected() const
{
	return _isSelected;
}

CustomPhysicsSprite::BallTextureType CustomPhysicsSprite::getBallTextureType() const
{
	return _ballTextureType;
}

void CustomPhysicsSprite::update(const float dt)
{
	if (_destroyFlag)
		return;

	b2Body* body = this->getB2Body();

	float thisX = this->getPosition().x / PTM_RATIO;
	float thisY = this->getPosition().y / PTM_RATIO;
	float centerX = Director::getInstance()->getWinSize().width / 2 / PTM_RATIO;
	float centerY = Director::getInstance()->getWinSize().height / 2 / PTM_RATIO;

	float GM = CustomPhysicsSprite::GM;

	float R2 = powf(thisX-centerX, 2.0f) + powf(thisY-centerY, 2.0f);
	float g = GM / R2;

	float R = sqrtf(R2);

	float gX = g * (centerX - thisX) / R;
	float gY = g * (centerY - thisY) / R;

	body->ApplyForce(b2Vec2(body->GetMass()*gX, body->GetMass()*gY), body->GetPosition(), false);
}


// 画面外に設置された場合、テクスチャがレンダリングされない（Sprite::draw）ので、
//  ここでdrawをオーバライドして、Sprite::drawに処理がわたらないようにする。
void CustomPhysicsSprite::draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags)
{
	if (this->_pB2Body->IsAwake())
	{
		if (isDirty())
		{
			syncPhysicsTransform();
		}
		_quadCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, &_quad, 1, _transform);
		renderer->addCommand(&_quadCommand);
	}
	else
	{
		extension::PhysicsSprite::draw(renderer, transform, flags);
	}

}

// デフォルトでは、親ノードの影響を受けないため、必ず絶対座標に基準になってしまっていた。
// なので、ここでは相対座標になるようにオーバーライドしている。
void CustomPhysicsSprite::syncPhysicsTransform() const
{
    b2Vec2 pos  = _pB2Body->GetPosition();

	float x = pos.x * _PTMRatio;
	float y = pos.y * _PTMRatio;

	if (_ignoreAnchorPointForPosition)
    {
		x += _anchorPointInPoints.x;
		y += _anchorPointInPoints.y;
	}

	// Make matrix
	float radians = _pB2Body->GetAngle();
	float c = cosf(radians);
	float s = sinf(radians);

	if (!_anchorPointInPoints.equals(Vec2::ZERO))
    {
		x += ((c * -_anchorPointInPoints.x * _scaleX) + (-s * -_anchorPointInPoints.y * _scaleY));
		y += ((s * -_anchorPointInPoints.x * _scaleX) + (c * -_anchorPointInPoints.y * _scaleY));

		// 追加
		x = x * this->getParent()->getScaleX() + this->getParent()->getPositionX() + (this->getParent()->getContentSize().width - this->getParent()->getScaleX() * this->getParent()->getContentSize().width) / 2;
		y = y * this->getParent()->getScaleY() + this->getParent()->getPositionY() + (this->getParent()->getContentSize().height - this->getParent()->getScaleY() * this->getParent()->getContentSize().height) / 2;
	}

	// Rot, Translate Matrix
	// 変更
    float mat[] = {  (float)c * _scaleX * this->getParent()->getScaleX(), (float)s * _scaleX * this->getParent()->getScaleX(), 0,  0,
        (float)-s * _scaleY * this->getParent()->getScaleY(), (float)c * _scaleY * this->getParent()->getScaleY(),  0,  0,
        0,  0,  1,  0,
        x,	y,  0,  1};

    _transform.set(mat);

}