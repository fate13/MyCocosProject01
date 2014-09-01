//
//  PleaseWaitAnimation.cpp
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/09/01.
//
//

#include "PleaseWaitAnimation.h"

FiniteTimeAction* PleaseWaitAnimation::loopAnimation()
{
	RotateBy* rotate = RotateBy::create(5, 90);

	RepeatForever* repeat = RepeatForever::create(rotate);

	return repeat;
}

FiniteTimeAction* PleaseWaitAnimation::outAnimation(std::function<void()> callBackFunc)
{
	Size winSize = Director::getInstance()->getWinSize();

	MoveTo* move = MoveTo::create(1, Vec2(winSize.width / 2, winSize.height * 1.5));
	CallFunc* func = CallFunc::create(callBackFunc);
	Sequence* sequence = Sequence::create(move, func, nullptr);

	return sequence;
}

FiniteTimeAction* PleaseWaitAnimation::inAnimation(std::function<void()> callBackFunc)
{
	Size winSize = Director::getInstance()->getWinSize();

	Place* place = Place::create(Vec2(winSize.width / 2, -winSize.height * 0.5));
	MoveTo* move = MoveTo::create(3, Vec2(winSize.width / 2, winSize.height / 2));
	CallFunc* func = CallFunc::create(callBackFunc);
	Sequence* sequence = Sequence::create(place, move, func, nullptr);

	return sequence;
}