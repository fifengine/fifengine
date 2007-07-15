%module(directors="1") guibase
%{
#include "gui/fonts/sdltruetypefont.hpp"
#include "gui/fonts/aafont.h"
#include "gui/fonts/guichanfont.h"

%}
typedef unsigned char Uint8;

namespace gcn {
    class Font
    {
    public:
        virtual ~Font(){ }
        virtual int getWidth(const std::string& text) const = 0;
        virtual int getHeight() const = 0;
	};
}

namespace FIFE {
	class FontBase: public gcn::Font {
	public:
		virtual void setColor(uint8_t r,uint8_t g,uint8_t b) = 0;
	};
}

namespace gcn {
	%rename(TTFont) SDLTrueTypeFont;
	class SDLTrueTypeFont: public FIFE::FontBase {
	public:
		SDLTrueTypeFont (const std::string& filename, int size);
		virtual ~SDLTrueTypeFont();
		virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
        virtual int getWidth(const std::string& text) const;
        virtual int getHeight() const;
	};
}
namespace FIFE {
	class AAImageFont: public FontBase {
		public:
			AAImageFont(const std::string& filename);
			virtual ~AAImageFont();
			virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
			virtual int getWidth(const std::string& text) const;
			virtual int getHeight() const;
	};

	%rename(ImageFont) GuichanImageFont;
	class GuichanImageFont: public FontBase {
		public:
			GuichanImageFont(const std::string& filename, const std::string& glyphs);;
			virtual ~GuichanImageFont();
			virtual void setColor(Uint8 r, Uint8 g, Uint8 b);
			virtual int getWidth(const std::string& text) const;
			virtual int getHeight() const;
	};
}