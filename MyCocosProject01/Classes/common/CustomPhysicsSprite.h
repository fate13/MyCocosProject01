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

USING_NS_CC;

class CustomPhysicsSprite : public extension::PhysicsSprite
{
public:

	void setUp(b2World* world, Vec2 position);
	void update(float dt);

	virtual void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags) override;

	static CustomPhysicsSprite* createWithSpriteFrameName(const char *pszSpriteFrameName);
};

#endif /* defined(__MyCocosProject01__CustomPhysicsSprite__) */
