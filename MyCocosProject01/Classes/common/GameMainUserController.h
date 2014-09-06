//
//  GameMainUserController.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/05.
//
//

#ifndef __MyCocosProject01__GameMainUserController__
#define __MyCocosProject01__GameMainUserController__

#include "cocos2d.h"

USING_NS_CC;

class GameMainUserController
{
public:
	bool touchBegan(Touch* touch, Event* event, Node* obj);
	void touchMoved(Touch* touch, Event* event, Node* obj);
	void touchEnded(Touch* touch, Event* event, Node* obj);

	~GameMainUserController();
};

#endif /* defined(__MyCocosProject01__GameMainUserController__) */
