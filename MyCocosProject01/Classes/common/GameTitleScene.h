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
#include "CustomLayer.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class GameTitleScene : public CustomLayer
{

public:

    static Scene* createScene();

	CREATE_FUNC(GameTitleScene);

	virtual void onEnterTransitionDidFinish() override;

	GameTitleScene();
	virtual ~GameTitleScene() override;


protected:

	virtual void sceneSetting(const std::string imageName) override;


private:

	std::string textureAtlasName;

	void createBG();
	void createStartButton();
	void touchEvent_startButton(Ref *pSender, ui::Widget::TouchEventType type);	

};

#endif /* defined(__MyCocosProject01__GameTitleScene__) */
