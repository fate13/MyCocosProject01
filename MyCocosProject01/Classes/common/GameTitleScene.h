//
//  GameTitleScene.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#ifndef __MyCocosProject01__GameTitleScene__
#define __MyCocosProject01__GameTitleScene__

#include "cocos2d.h"

USING_NS_CC;

class GameTitleScene : public Layer
{
public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init() override;

    // implement the "static create()" method manually
    CREATE_FUNC(GameTitleScene);

};

#endif /* defined(__MyCocosProject01__GameTitleScene__) */
