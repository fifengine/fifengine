/***************************************************************************
 *   Copyright (C) 2005-2017 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

// Standard C++ library includes
#include <vector>

// Platform specific includes

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/structures/rect.h"
#include "util/base/exception.h"
#include "util/utf8/utf8.h"
#include "video/image.h"
#include "video/renderbackend.h"

#include "fontbase.h"

namespace FIFE {

	FontBase::FontBase():
			m_pool(),
			mColor(),
			mGlyphSpacing(0),
			mRowSpacing(0),
			mFilename(""),
			m_antiAlias(true),
			m_boldStyle(false),
			m_italicStyle(false),
			m_underlineStyle(false),
			m_strikeStyle(false),
			m_coloring(false) {
	}

	void FontBase::invalidate() {
		m_pool.invalidateCachedText();
	}

	void FontBase::setRowSpacing(int32_t spacing) {
		mRowSpacing = spacing;
	}

	int32_t FontBase::getRowSpacing() const {
		return mRowSpacing;
	}

	void FontBase::setGlyphSpacing(int32_t spacing) {
		mGlyphSpacing = spacing;
	}

	int32_t FontBase::getGlyphSpacing() const {
		return mGlyphSpacing;
	}

	void FontBase::setAntiAlias(bool antiAlias) {
		m_antiAlias = antiAlias;
	}

	bool FontBase::isAntiAlias() const {
		return m_antiAlias;
	}

	void FontBase::setBoldStyle(bool style) {
		m_boldStyle = style;
	}

	bool FontBase::isBoldStyle() const {
		return m_boldStyle;
	}

	void FontBase::setItalicStyle(bool style) {
		m_italicStyle = style;
	}

	bool FontBase::isItalicStyle() const {
		return m_italicStyle;
	}

	void FontBase::setUnderlineStyle(bool style) {
		m_underlineStyle = style;
	}

	bool FontBase::isUnderlineStyle() const {
		return m_underlineStyle;
	}

	void FontBase::setStrikethroughStyle(bool style) {
		m_strikeStyle = style;
	}

	bool FontBase::isStrikethroughStyle() const {
		return m_strikeStyle;
	}

	void FontBase::setDynamicColoring(bool coloring) {
		m_coloring = coloring;
	}
	
	bool FontBase::isDynamicColoring() const {
		return m_coloring;
	}

	SDL_Color FontBase::getColor() const {
		return mColor;
	}

	int32_t FontBase::getStringIndexAt(const std::string &text, int32_t x) const {
		assert( utf8::is_valid(text.begin(), text.end()) );
		std::string::const_iterator cur;
		if (text.size() == 0) return 0;
		if (x <= 0) return 0;

		cur = text.begin();

		utf8::next(cur, text.end());

		std::string buff;
		while(cur != text.end()) {
			buff = std::string(text.begin(), cur);

			if (getWidth(buff) > x) {
				return buff.size();
			} else {
				utf8::next(cur, text.end());
			}
		}

		if (x > getWidth(text)) {
			return text.size();
		} else {
			return buff.size();
		}
	}

	Image* FontBase::getAsImage(const std::string& text) {
		Image* image = m_pool.getRenderedText(this, text);
		if (!image) {
			SDL_Surface* textSurface = renderString(text);
			image = RenderBackend::instance()->createImage(textSurface);
			m_pool.addRenderedText( this, text, image );
		}
		return image;
	}

	Image* FontBase::getAsImageMultiline(const std::string& text) {
		const uint8_t newline_utf8 = '\n';
		uint32_t newline;
		utf8::utf8to32(&newline_utf8,&newline_utf8 + 1,&newline);
		//std::cout << "Text:" << text << std::endl;
		Image* image = m_pool.getRenderedText(this, text);
		if (!image) {
			std::vector<SDL_Surface*> lines;
			std::string::const_iterator it = text.begin();
			// split text as needed
			int32_t render_width = 0, render_height = 0;
			do {
				uint32_t codepoint = 0;
				std::string line;
				while( codepoint != newline && it != text.end() )
				{
					codepoint = utf8::next(it,text.end());
					if( codepoint != newline )
						utf8::append(codepoint, back_inserter(line));
				}
				//std::cout << "Line:" << line << std::endl;
				SDL_Surface* text_surface = renderString(line);
				if (text_surface->w > render_width) {
					render_width = text_surface->w;
				}
				lines.push_back(text_surface);
			} while (it != text.end());

			render_height = (getRowSpacing() + getHeight()) * lines.size();
			SDL_Surface* final_surface = SDL_CreateRGBSurface(0,
				render_width,render_height,32,
				RMASK, GMASK, BMASK ,AMASK);
			if (!final_surface) {
				throw SDLException(std::string("CreateRGBSurface failed: ") + SDL_GetError());
			}
			SDL_FillRect(final_surface, 0, 0x00000000);
			int32_t ypos = 0;
			for (std::vector<SDL_Surface*>::iterator i = lines.begin(); i != lines.end(); ++i) {
				SDL_Rect dst_rect = { 0, 0, 0, 0 };
				dst_rect.y = ypos;

				// Disable alpha blending
				//SDL_SetAlpha(*i,0,SDL_ALPHA_OPAQUE);
				SDL_SetSurfaceBlendMode(*i, SDL_BLENDMODE_NONE);
				SDL_BlitSurface(*i, 0, final_surface, &dst_rect);
				ypos += getRowSpacing() + getHeight();
				SDL_FreeSurface(*i);
			}
			image = RenderBackend::instance()->createImage(final_surface);
			m_pool.addRenderedText(this, text, image);
		}
		return image;
	}

	std::string FontBase::splitTextToWidth (const std::string& text, int32_t render_width) {
		const uint32_t whitespace = ' ';
		const uint8_t newline_utf8 = '\n';
		uint32_t newline;
		utf8::utf8to32(&newline_utf8,&newline_utf8 + 1,&newline);
		if (render_width <= 0 || text.empty()) {
			return text;
		}
		std::string output;
		std::string line;
		std::string::const_iterator pos = text.begin();
		std::list<std::pair<size_t,std::string::const_iterator> > break_pos;
		bool firstLine = true;

		while( pos != text.end())
		{
			break_pos.clear();
			if( !firstLine ) {
				line =  "\n";
			} else {
				firstLine = false;
			}

			bool haveNewLine = false;
			while( getWidth(line) < render_width && pos != text.end() )
			{
				uint32_t codepoint = utf8::next(pos, text.end());
				if (codepoint == whitespace && !line.empty())
					break_pos.push_back( std::make_pair(line.length(),pos) );

				if( codepoint != newline )
					utf8::append(codepoint, back_inserter(line) );

				// Special case: Already newlines in string:
				if( codepoint == newline ) {
					output.append(line);
					line = "";
					haveNewLine = true;
					break;
				}
			}
			if( haveNewLine )
				continue;

			if( pos == text.end() )
				break;

			if( break_pos.empty() ) {
				// No break position and line length smaller than 2
				// means the renderwidth is really screwed. Just continue
				// appending single character lines.
				if( utf8::distance(line.begin(),line.end()) <= 1 && line != "\n") {
					output.append(line);
					continue;
				}

				if (line == "\n") {
					++pos;
				}

				// We can't do hyphenation here,
				// so we just retreat one character :-(
				// FIXME
				//line = line.erase(line.length() - 1);
				//--pos;
			} else {
				line = line.substr(0,break_pos.back().first);
				pos = break_pos.back().second;
			}
			output.append(line);
		}
		if( !line.empty() ) {
			output.append(line);
		}
		return output;
	}
}
