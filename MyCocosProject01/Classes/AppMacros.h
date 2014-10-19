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

static Resource smallResource = {cocos2d::Size(320, 480), "S"};
static Resource mediumResource = {cocos2d::Size(640, 960), "M"};
static Resource largeResource = {cocos2d::Size(1366, 2048), "L"};

static int largeResourceThreshold = 900;

static cocos2d::Size designResolutionSize = cocos2d::Size(640, 960);


#define FONT_SIZE(__SIZE) (cocos2d::Director::getInstance()->getOpenGLView()->getDesignResolutionSize().width / mediumResource.size.width * __SIZE)

#define WIN_POS(__X, __Y) (cocos2d::Vec2(designResolutionSize.width * __X, designResolutionSize.height * __Y))

#define GL_FRAME_OFFSET_Y 

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	#define IMAGE_NAME(__NAME) (__NAME + ".png")
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	#define IMAGE_NAME(__NAME) (__NAME + ".png")
#endif


#define CUSTOM_COLOR_YELLOW Color3B(255, 255, 54)
#define CUSTOM_COLOR_BLUE Color3B(163, 255, 255)
#define CUSTOM_COLOR_GREEN Color3B(147, 255, 155)



#endif
