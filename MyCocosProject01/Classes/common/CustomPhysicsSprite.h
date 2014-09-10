//
//  CustomPhysicsSprite.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/04.
//
//

#ifndef __MyCocosProject01__CustomPhysicsSprite__
#define __MyCocosProject01__CustomPhysicsSprite__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"
#include "BOX2D/BOX2D.h"
#include "GameMainModel.h"

USING_NS_CC;

class CustomPhysicsSprite : public extension::PhysicsSprite
{
public:

	static float GM;

	enum class BallTextureType
	{
		Normal,
		Selected,
		Disabled
	};

	CustomPhysicsSprite();

	void setUp(b2World* world, const std::shared_ptr<GameMainModel::BallInfo> ballInfo);
	void update(const float dt);

	void changeTexture(const BallTextureType ballTextureType);
	void destroy(b2World* world);


	int getId() const;
	void setNumber(const int number);
	int getNumber() const;
	void setPresence(const bool isPresence);
	bool isPresence() const;
	void setSelectEnable(const bool isSselectEnable);
	bool isSelectEnable() const;
	void setSelected(const bool isSelected);
	bool isSelected() const;
	BallTextureType getBallTextureType() const;

	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;

	static CustomPhysicsSprite* createWithSpriteFrameName(const char *pszSpriteFrameName);

	static CustomPhysicsSprite* createWithBallNumber(const int ballNumber);

	
private:
	int _id;
	int _number;
	bool _isPresence;
	bool _isSelectEnable;
	bool _isSelected;
	BallTextureType _ballTextureType;
	bool _destroyFlag;

};

#endif /* defined(__MyCocosProject01__CustomPhysicsSprite__) */
