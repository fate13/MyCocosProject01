//
//  CustomLayer.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/01.
//
//

#ifndef __MyCocosProject01__CustomLayer__
#define __MyCocosProject01__CustomLayer__

#include "cocos2d.h"

USING_NS_CC;

using namespace ui;

class CustomLayer : public Layer
{
private:

	void createBG();
	void createStartButton();


protected:

	virtual void pleaseWaitLayer_set();
	virtual void pleaseWaitLayer_out();
	virtual void pleaseWaitLayer_in(std::function<void()> callBackFunc);

	virtual void pleaseWaitAnimation_loop(LayerColor* overLayer);
	virtual void pleaseWaitAnimation_out(LayerColor* overLayer, std::function<void()> callBackFunc);
	virtual void pleaseWaitAnimation_in(LayerColor* overLayer, std::function<void()> callBackFunc);

	virtual void loadTextureAtlasAsync(const std::string imageName);
	virtual void loadTextureAtlasAsyncComplete(Texture2D* texture, const std::string imageName);
	virtual void sceneSetting(const std::string imageName);
	virtual void dispose(const std::string imageName);
	virtual void gotoNextScene(Scene* nextScene);
	virtual void replaceScene();

	Scene* _nextScene;

public:

    static Scene* createScene();

    virtual bool init() override;

    CREATE_FUNC(CustomLayer);

	virtual void onEnterTransitionDidFinish() override;

	virtual ~CustomLayer() override;
	
};

#endif /* defined(__MyCocosProject01__CustomLayer__) */
