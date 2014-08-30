//
//  AppMacros.h
//  MyCocosProject01
//
//  Created by HiroyukiSasaki on 2014/08/31.
//
//

#ifndef MyCocosProject01_AppMacros_h
#define MyCocosProject01_AppMacros_h

#include "cocos2d.h"

typedef struct tagResource
{
	cocos2d::Size size;
	char directory[10];
} Resource;

static Resource smallResource = {cocos2d::Size(320, 568), "S"};
static Resource mediumResource = {cocos2d::Size(640, 1136), "M"};
static Resource largeResource = {cocos2d::Size(1154, 2048), "L"};

static cocos2d::Size designResolutionSize = cocos2d::Size(640, 1136);

#define FONT_SIZE(__SIZE) (cocos2d::Director::getInstance()->getOpenGLView()->getDesignResolutionSize().width / mediumResource.size.width * __SIZE)

#endif
