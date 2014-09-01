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

USING_NS_CC;

class GameMainScene : public CustomLayer
{
private:



protected:

	virtual void sceneSetting(const std::string imageName) override;


public:

    static Scene* createScene();

	CREATE_FUNC(GameMainScene);

	virtual void onEnterTransitionDidFinish() override;

	virtual ~GameMainScene() override;
	
};

#endif /* defined(__MyCocosProject01__GameMainScene__) */
