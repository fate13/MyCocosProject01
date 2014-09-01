//
//  PleaseWaitAnimation.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/01.
//
//

#ifndef __MyCocosProject01__PleaseWaitAnimation__
#define __MyCocosProject01__PleaseWaitAnimation__

#include "cocos2d.h"

USING_NS_CC;

class PleaseWaitAnimation
{
public:

	static FiniteTimeAction* loopAnimation();
	static FiniteTimeAction* outAnimation(std::function<void()> callBackFunc);
	static FiniteTimeAction* inAnimation(std::function<void()> callBackFunc);

};

#endif /* defined(__MyCocosProject01__PleaseWaitAnimation__) */
