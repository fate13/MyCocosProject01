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
private:

	void createBG();
	void createStartButton();
	void touchEvent_startButton(Ref *pSender, ui::Widget::TouchEventType type);


protected:

	virtual void sceneSetting(const std::string imageName) override;

	
public:

    static Scene* createScene();

	CREATE_FUNC(GameTitleScene);

	virtual void onEnterTransitionDidFinish() override;

};

#endif /* defined(__MyCocosProject01__GameTitleScene__) */
