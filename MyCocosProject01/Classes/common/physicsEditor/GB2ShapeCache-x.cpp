//
//  GB2ShapeCache-x.cpp
//
//  Loads physics sprites created with http://www.PhysicsEditor.de
//  To be used with cocos2d-x
//
//  Generic Shape Cache for box2d
//
//  Created by Thomas Broquist
//
//      http://www.PhysicsEditor.de
//      http://texturepacker.com
//      http://www.code-and-web.de
//
//    Copyright (c) 2012 Chris Hannon / channon.us
//    Copyright (c) 2013 Tomoaki Shimizu / tks2.net
//
//  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include "GB2ShapeCache-x.h"
#include "Box2D/Box2D.h"
//#include "cocoa/CCNS.h"

using namespace cocos2d;
using namespace gbox2d;

/**
 * Internal class to hold the fixtures
 */
class FixtureDef {
public:
    FixtureDef()
    : next(nullptr) {}
    
    ~FixtureDef() {
        delete next;
        delete fixture.shape;
    }
    
    FixtureDef *next;
    b2FixtureDef fixture;
    int callbackData;
};

class BodyDef {
public:
    BodyDef()
    : fixtures(nullptr) {}
    
    ~BodyDef() {
        if (fixtures)
            delete fixtures;
    }
    
    FixtureDef *fixtures;
    Point anchorPoint;
};

static GB2ShapeCache *_sharedGB2ShapeCache = nullptr;

GB2ShapeCache* GB2ShapeCache::getInstance(void) {
    if (!_sharedGB2ShapeCache) {
        _sharedGB2ShapeCache = new GB2ShapeCache();
        _sharedGB2ShapeCache->init();
    }
    
    return _sharedGB2ShapeCache;
}

bool GB2ShapeCache::init() {
    return true;
}

void GB2ShapeCache::reset() {
    std::map<std::string, BodyDef *>::iterator iter;
    for (iter = shapeObjects.begin() ; iter != shapeObjects.end() ; ++iter) {
        delete iter->second;
    }
    shapeObjects.clear();
}

void GB2ShapeCache::addFixturesToBody(b2Body *body, const std::string &shape) {
    std::map<std::string, BodyDef *>::iterator pos = shapeObjects.find(shape);
    assert(pos != shapeObjects.end());
    
    auto so = (*pos).second;
    
    auto fix = so->fixtures;
    while (fix) {
        body->CreateFixture(&fix->fixture);
        fix = fix->next;
    }
}

cocos2d::Point GB2ShapeCache::anchorPointForShape(const std::string &shape) {
    auto pos = shapeObjects.find(shape);
    assert(pos != shapeObjects.end());
    
    auto bd = (*pos).second;
    return bd->anchorPoint;
}

void GB2ShapeCache::addShapesWithFile(const std::string &plist) {

	ValueMap dict = FileUtils::getInstance()->getValueMapFromFile(plist);

    CCAssert(dict.size() != 0, "plist file empty or not existing");
    
    ValueMap metadataDict = dict.at("metadata").asValueMap();
    int format = metadataDict.at("format").asInt();
    ptmRatio = metadataDict.at("ptm_ratio").asFloat();
    CCAssert(format == 1, "Format not supported");
    
    ValueMap bodyDict = dict.at("bodies").asValueMap();
    
    b2Vec2 vertices[b2_maxPolygonVertices];

	for (std::pair<std::string, Value> element : bodyDict)
	{
        auto bodyDef = new BodyDef();
        
        std::string bodyName = element.first;
        
        ValueMap bodyData = element.second.asValueMap();
        bodyDef->anchorPoint = PointFromString(bodyData.at("anchorpoint").asString());
        
        ValueVector fixtureList = bodyData.at("fixtures").asValueVector();
        auto nextFixtureDef = &(bodyDef->fixtures);

        for(Value fixture : fixtureList)
        {
            b2FixtureDef basicData;
            ValueMap fixtureData = fixture.asValueMap();
            int callbackData = 0;
            
            basicData.filter.categoryBits = fixtureData.at("filter_categoryBits").asInt();
            basicData.filter.maskBits = fixtureData.at("filter_maskBits").asInt();
            basicData.filter.groupIndex = fixtureData.at("filter_groupIndex").asInt();
            basicData.friction = fixtureData.at("friction").asFloat();
            basicData.density = fixtureData.at("density").asFloat();
            basicData.restitution = fixtureData.at("restitution").asFloat();
            basicData.isSensor = fixtureData.at("isSensor").asBool();
            if(fixtureData.at("id").asString() != ""){
				std::string userData = fixtureData.at("id").asString();
                basicData.userData = &userData;
                callbackData = fixtureData.at("id").asInt();
            }
            
            std::string fixtureType = fixtureData.at("fixture_type").asString();
            
            if (fixtureType == "POLYGON") {

				ValueVector polygons = fixtureData.at("polygons").asValueVector();

				for(Value polygon : polygons)
                {
                    auto fix = new FixtureDef();
                    fix->fixture = basicData; // copy basic data
                    fix->callbackData = callbackData;
                    
                    b2PolygonShape *polyshape = new b2PolygonShape();
                    int vindex = 0;

                    ValueVector polygonData = polygon.asValueVector();
                    
                    assert(polygonData.size() <= b2_maxPolygonVertices);

                    for(Value offset : polygonData)
                    {
                        
						std::string pStr = offset.asString();
                        auto p = PointFromString(pStr);
                        
                        vertices[vindex].x = (p.x / ptmRatio) ;
                        vertices[vindex].y = (p.y / ptmRatio) ;
                        vindex++;
                        
                    }
                    
                    polyshape->Set(vertices, vindex);
                    fix->fixture.shape = polyshape;
                    
                    *nextFixtureDef = fix;
                    nextFixtureDef = &(fix->next);
                }
                
            } else if (fixtureType == "CIRCLE") {
                auto fix = new FixtureDef();
                fix->fixture = basicData; // copy basic data
                fix->callbackData = callbackData;
                
                ValueMap circleData = fixtureData.at("circle").asValueMap();
                
                auto circleShape = new b2CircleShape();
                
                circleShape->m_radius = circleData.at("radius").asFloat() / ptmRatio;
                auto p = PointFromString(circleData.at("position").asString());
                circleShape->m_p = b2Vec2(p.x / ptmRatio, p.y / ptmRatio);
                fix->fixture.shape = circleShape;
                
                // create a list
                *nextFixtureDef = fix;
                nextFixtureDef = &(fix->next);
                
            } else {
                CCAssert(0, "Unknown fixtureType");
            }
            
            // add the body element to the hash
            shapeObjects[bodyName] = bodyDef;
            
        }
        
    }
    
}
