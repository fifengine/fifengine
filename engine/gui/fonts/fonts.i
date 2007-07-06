%module(directors="1") guibase
%{
#include "gui/fonts/sdltruetypefont.hpp"
#include "gui/fonts/aafont.h"
#include "gui/fonts/guichanfont.h"

%}
typedef unsigned char Uint8;

namespace gcn {
	%rename(TTFont) SDLTrueTypeFont;
	class SDLTrueTypeFont {
	public:
		SDLTrueTypeFont (const std::string& filename, int size);
		virtual ~SDLTrueTypeFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
	};
}
namespace FIFE {
	class AAImageFont {
		public:
			AAImageFont(const std::string& filename);
			virtual ~AAImageFont();
			virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
	};

	%rename(ImageFont) GuichanImageFont;
	class GuichanImageFont {
		public:
			GuichanImageFont(const std::string& filename, const std::string& glyphs);;
			virtual ~GuichanImageFont();
			virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
	};
}