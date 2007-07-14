%module(directors="1") guibase
%{
#include <guichan.hpp>
#include "gui/base/advimage.h"
%}

namespace gcn {
	class Color
	{
	public:
		Color();
		Color(int color);
		Color(int r, int g, int b, int a = 255);
		int r;
		int g;
		int b;
		int a;
	};

	%feature("director") ImageAnimationListener;
	class ImageAnimationListener {
	public:
		virtual void onAnimationEnd() = 0;
		virtual ~ImageAnimationListener();
	};

	class AdvImage {
	public:
		AdvImage();
		AdvImage(const std::string& filename);
		~AdvImage();
		int getWidth() const;
		int getHeight() const;
		void setAnimActive(bool active);
		void setAnimDirection(bool forward);
		void setAnimListener(ImageAnimationListener* animationlistener);
		void removeAnimListener();
		bool isAnimation();

		void loadFromFile(const std::string & filename);
		void loadFromCache(const size_t id);
	};
}

