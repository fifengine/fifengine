/***************************************************************************
 *   Copyright (C) 2005-2008 by the FIFE team                              *
 *   http://www.fifengine.de                                               *
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
#include <boost/filesystem/convenience.hpp>

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
			m_antiAlias(true) {
	}

	void FontBase::setRowSpacing(int spacing) {
		mRowSpacing = spacing;
	}

	int FontBase::getRowSpacing() const {
		return mRowSpacing;
	}

	void FontBase::setGlyphSpacing(int spacing) {
		mGlyphSpacing = spacing;
	}

	int FontBase::getGlyphSpacing() const {
		return mGlyphSpacing;
	}

	void FontBase::setAntiAlias(bool antiAlias) {
		m_antiAlias = antiAlias;
	}

	bool FontBase::isAntiAlias() {
		return m_antiAlias;
	}

	SDL_Color FontBase::getColor() const {
		return mColor;
	}

	int FontBase::getStringIndexAt(const std::string &text, int x) {
		std::string::const_iterator cur, last;
		int idx = 0;
		if (text.size() == 0) return 0;
		if (x <= 0) return 0;

		last = text.begin();
		cur = last;

		utf8::next(cur, text.end());

		std::string buff;
		while(cur != text.end()) {
			buff = std::string(text.begin(), cur);

			if (getWidth(buff) > x) {
				return buff.size();
			} else {
				utf8::next(cur, text.end());
				idx++;
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
		//FIXME UTF8 support
		Image* image = m_pool.getRenderedText(this, text);
		if (!image) {
			std::vector<SDL_Surface*> lines;

			// split text as needed
			std::string::size_type pos, last_pos = 0;
			int length = 0;
			int render_width = 0, render_height = 0;
			do {
				pos = text.find('\n', last_pos);
				if (pos != std::string::npos) {
					length = pos - last_pos;
				} else {
					length = text.size() - last_pos;
				}
				std::string sub = text.substr(last_pos, length);
				std::cerr << "substring: " << sub << "\n";
				SDL_Surface* text_surface = renderString(sub);
				if (text_surface->w > render_width) {
					render_width = text_surface->w;
				}
				lines.push_back(text_surface);
				last_pos = pos + 1;
			} while (pos != std::string::npos);
			render_height = (getRowSpacing() + getHeight()) * lines.size();
			SDL_Surface* final_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
				render_width,render_height,32,
				RMASK, GMASK, BMASK ,AMASK);
			if (!final_surface) {
				std::cerr << "CreateRGBSurface failed: " << SDL_GetError() << "\n";
				exit(0);
			}
			SDL_FillRect(final_surface, 0, 0x00000000);
			int ypos = 0;
			for (std::vector<SDL_Surface*>::iterator i = lines.begin(); i != lines.end(); ++i) {
				SDL_Rect dst_rect = { 0, 0, 0, 0 };
				dst_rect.y = ypos;

				SDL_SetAlpha(*i,0,SDL_ALPHA_OPAQUE);
				SDL_BlitSurface(*i,0,final_surface,&dst_rect);
				ypos += getRowSpacing() + getHeight();
				SDL_FreeSurface(*i);
			}
			image = RenderBackend::instance()->createImage(final_surface);
			m_pool.addRenderedText(this, text, image);
		}
		return image;
	}

	std::string FontBase::splitTextToWidth (const std::string& text, int render_width) {
		if (render_width <= 0 || text.empty()) { 
			return text;
		}
		std::string output;
		std::string line;
		std::string::size_type pos = 0;
		std::list<std::pair<size_t,size_t> > break_pos;
		bool firstLine = true;

		while( pos < text.length() )
		{
			break_pos.clear();
			if( !firstLine ) {
				line =  "\n";
			} else {
				firstLine = false;
			}

			bool haveNewLine = false;
			while( getWidth(line) < render_width && pos < text.length() )
			{
				if (text.at(pos) == ' ' && !line.empty())
					break_pos.push_back( std::make_pair(line.length(),pos) );
				line.push_back( text.at(pos) );
				++pos;

				// Special case: Already newlines in string:
				if( text.at(pos-1) == '\n' ) {
					if( line[line.size()-1] == '\n' )
						line.erase(line.size()-1);
					output.append(line);
					line = "";
					haveNewLine = true;
					break;
				}
			}
			if( haveNewLine )
				continue;

			if( pos >= text.length())
				break;

			if( break_pos.empty() ) {
				// No break position and line length smaller than 2
				// means the renderwidth is really screwed. Just continue
				// appending single character lines.
				if( line.length() <= 1 ) {
					output.append(line);
					continue;
				}

				// We can't do hyphenation here,
				// so we just retreat one character :-(
				line.erase(line.length() - 1);
				--pos;
			} else {
				line = line.substr(0,break_pos.back().first);
				pos = break_pos.back().second + 1;
			}
			output.append(line);
		}
		if( !line.empty() ) {
			output.append(line);
		}
		return output;
	}
}
