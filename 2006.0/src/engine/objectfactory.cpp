#include "objectfactory.h"
#include "object.h"
#include "visual.h"
namespace FIFE {

Object* ObjectFactory::createStaticImage(const std::string& name, size_t imageid)
{
	Object* obj = new Object(name);
	ObjectFactory::makeStaticImage(obj,imageid);
	return obj;
};

Object* ObjectFactory::createTileOverlay(const std::string& name, size_t imageid)
{
	Object* obj = new Object(name);
	ObjectFactory::makeStaticImage(obj,imageid);
	return obj;
};

Object* ObjectFactory::createTileOverlay(const std::string& name, Animation* anim)
{
	Object* obj = new Object(name);
	Visual* vis = obj->getVisual();
	vis->setAnimation(anim);
	vis->setPositionType( Visual::PT_TILE_GRID);
	return obj;
};

void ObjectFactory::makeStaticImage( Object* obj, size_t imageid, int xoff, int yoff) {
	obj->getVisual()->setImage(imageid,xoff,yoff);
};

void ObjectFactory::makeStaticAnimation( Object* obj, Animation* anim, int xoff, int yoff) {
	Visual* vis = obj->getVisual();
	vis->setAnimation(anim);
	vis->setShift(xoff,yoff);
};

};
