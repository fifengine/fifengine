#ifndef OBJECTFACTORY_H
#define OBJECTFACTORY_H
#include <string>

namespace FIFE {

	class Object;
	class Image;
	class Animation;

	class ObjectFactory  {
		public:
			static Object* createStaticImage(const std::string& name, size_t imageid);
			//static Object* createStaticImage(const std::string& name, Animation* anim);
			static Object* createTileOverlay(const std::string& name, Animation* anim);
			static Object* createTileOverlay(const std::string& name, size_t imageid);

			static void makeStaticImage( Object* obj, size_t imageid, int xoff =0, int yoff =0);
			static void makeStaticAnimation( Object* obj, Animation* anim, int xoff =0, int yoff =0);

	};
}; // FIFE

#endif // OBJECTFACTORY_H
