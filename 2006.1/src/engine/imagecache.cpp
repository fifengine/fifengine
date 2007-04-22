/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
 *   fife-public@lists.sourceforge.net                                     *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or modify          *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA              *
 ***************************************************************************/

#include "imagecache.h"
#include "exception.h"
#include "video/renderable.h"

#include "maploaders/fallout/mffalloutfrm.h"
#include "maploaders/util/image_provider.h"
#include "maploaders/util/frm_provider.h"

#include "debugutils.h"

#include <algorithm>

namespace FIFE {

	class RenderableProviderConstructor {
		public:
			virtual ~RenderableProviderConstructor() {};
			virtual RenderableProviderPtr create(const RenderableLocation& location) = 0;
	};


	// FIXME: This Code should NOT be here!
	class FRMConstructor : public RenderableProviderConstructor {
		public:
			virtual ~FRMConstructor() {};
			virtual RenderableProviderPtr create(const RenderableLocation& location) {
				if( !location.isFile() )
					return RenderableProviderPtr();
				return RenderableProviderPtr(new FRMProvider(location));
			};
	};

	class IMGConstructor : public RenderableProviderConstructor {
		public:
			IMGConstructor() {};
			virtual ~IMGConstructor() {};
			virtual RenderableProviderPtr create(const RenderableLocation& location) {
				if( !location.isFile() )
					return RenderableProviderPtr();
				return RenderableProviderPtr(new ImageProvider(location));
			};
	};

	class DummyImage : public RenderAble {
			std::string m_msg;
		public:
			DummyImage(const std::string& msg = "") : RenderAble(RT_ANY), m_msg(msg) {};
			virtual ~DummyImage() {};

			virtual void render(const Rect& rect, Screen* screen, unsigned char alpha = 255) {};

			virtual unsigned int getWidth() const { return 0; };
			virtual unsigned int getHeight() const { return 0; };

			virtual void setXShift(int xshift) {};
			virtual void setYShift(int yshift) {};
			virtual int getXShift() const { return 0; };
			virtual int getYShift() const { return 0; };
	};

	static const size_t IC_PREALLOCATION = 1024 * 10;
	static const size_t IC_MIN_ALIVE = 1000;
	static const size_t IC_LOAD_CREDITS = 50;
	static const size_t IC_HIT_CREDITS = 1;
	static const size_t IC_INITIAL_CHUNK_SIZE = 100;

	ImageCache::ImageCache() : m_cache() {
		m_cache.reserve(IC_PREALLOCATION);

		addImage( new DummyImage() ); // SENTINEL
		m_stats.chunk_size = IC_INITIAL_CHUNK_SIZE;

		m_stats.alive = m_stats.loadable = 0;
		m_stats.cstart = m_stats.total = 0;
		m_stats.dummies = 0;

		m_providerconstructors.push_front( new IMGConstructor() );
		m_providerconstructors.push_front( new FRMConstructor() );
	};

	ImageCache::~ImageCache() {
		printStatistics();
		cleanUp();
	};

	void ImageCache::cleanUp() {
		cleanCache();
		cleanProviders();
	};

	void ImageCache::cleanCache() {
		type_cache::const_iterator end = m_cache.end();
		for (type_cache::iterator i = m_cache.begin(); i != end; ++i)
			delete i->renderable;

		m_cache.clear();
	}

	void ImageCache::cleanProviders() {
		type_providerconstructors::const_iterator end = m_providerconstructors.end();
		for(type_providerconstructors::iterator it = m_providerconstructors.begin(); it != end; ++it )
			delete *it;

		m_providerconstructors.clear();
	}

	void ImageCache::printStatistics() {
		std::cout
			<< "Cache:   " << m_cache.size() << std::endl
			<< "Total:   " << m_stats.total << std::endl
			<< "Dummies: " << m_stats.dummies << std::endl
			<< "Loadable:" << m_stats.loadable << std::endl;
	};

	size_t ImageCache::addImage(RenderAble* renderable) {
		if( renderable == 0 )
			return UNDEFINED_ID;

		size_t index = m_renderablemap[ renderable ];
		if( index )
			return index;

		index = addCacheEntry( RenderableLocation(), RenderableProviderPtr(), renderable );
		m_renderablemap[ renderable ] = index;
		return index;
	};

	size_t ImageCache::addImageFromFile( const std::string& filename ) {
		return addImageFromLocation( RenderableLocation( RenderableLocation::RL_FILE, filename ) );
	};

	size_t ImageCache::addImageFromLocation( const RenderableLocation& location ) {
		size_t index = m_locationmap[ location ];
		if( index )
			return index;

		index = addCacheEntry( location, RenderableProviderPtr(), 0 );
		m_locationmap[ location ] = index;
		return index;
	};

	size_t ImageCache::addCacheEntry( const RenderableLocation& location, RenderableProviderPtr provider, RenderAble* renderable ) {
		s_cache_entry centry;
		centry.location = location;
		centry.provider = provider;
		centry.renderable = renderable;
		centry.reloadable = provider || location;
		if( !centry.reloadable && !centry.renderable )
			PANIC_PRINT("renderable = NULL and location & provider are invalid!");

		if( centry.reloadable )
			++m_stats.loadable;

		++m_stats.total;

		if( centry.renderable ) {
			++m_stats.alive;
			centry.credits = IC_LOAD_CREDITS;
		};

		m_cache.push_back( centry );
		return m_cache.size() - 1;
	};

	RenderAble* ImageCache::getImage(size_t index) {
		if (m_cache.size() <= index)
			throw IndexOverflow( "index not valid." );

		s_cache_entry& centry = m_cache[ index ];
		if( centry.renderable ) {
			centry.credits += IC_HIT_CREDITS;
			return centry.renderable;
		};
		return loadImage( index );
	};

	RenderAble* ImageCache::loadImage( size_t index ) {
		s_cache_entry& centry = m_cache[ index ];
		if( centry.provider )
			centry.renderable = centry.provider->createRenderable();

		if (!centry.renderable && centry.location)
			findAndSetProvider(centry);

		if (!centry.renderable) {
			centry.renderable = new DummyImage(centry.location.toString());
			centry.provider = RenderableProviderPtr();
			centry.location = RenderableLocation();
			DEBUG_PRINT(centry.location.toString());
			--m_stats.loadable;
			++m_stats.dummies;
		}

// 		DEBUG_PRINT( centry.location.toString() );
		++m_stats.alive;
		if( !m_renderablemap[ centry.renderable ] )
			m_renderablemap[ centry.renderable ] = index;
		else {
			if( m_renderablemap[ centry.renderable ] == index )
				DEBUG_PRINT("double imagecache entry: " << centry.location.toString());
		}

		centry.credits = IC_LOAD_CREDITS;
		return centry.renderable;
	};

	void ImageCache::findAndSetProvider( s_cache_entry& centry ) {
		//DEBUG_PRINT( centry.location.toString() );
		type_providerconstructors::iterator it = m_providerconstructors.begin();
		type_providerconstructors::iterator end = m_providerconstructors.end();
		if( it == end )
			PANIC_PRINT( "no provider constructors given for image cache");
		for(; it != end; ++it) {
			RenderableProviderPtr p = (*it)->create( centry.location );
			if( !p )
				continue;

			try {
				centry.renderable = p->createRenderable();
			} catch(...) {
				continue;
			}

			if( !centry.renderable )
				continue;

			centry.provider = p;
			return;
		};
		// returns here, if centry.renderable stays zero.
	};

	void ImageCache::unloadImage( s_cache_entry& centry ) {
		//DEBUG_PRINT("unload image: " << centry.location.toString() );
		--m_stats.alive;
		delete centry.renderable;
		centry.renderable = 0;
		if( centry.provider )
			centry.provider->unload();
	};

	void ImageCache::collect() {
		if( m_stats.alive < IC_MIN_ALIVE )
			return;

		++m_stats.turns;
		if( m_stats.cstart + m_stats.chunk_size > m_stats.total ) {
			collectChunk( m_stats.cstart, m_stats.total , m_stats.turns);
			m_stats.cstart = 0;
			m_stats.turns = 0;
		} else {
			collectChunk( m_stats.cstart,m_stats.cstart + m_stats.chunk_size, m_stats.turns);
			m_stats.cstart += m_stats.chunk_size;
		};

	};

	void ImageCache::collectChunk(size_t start, size_t end, int turns) {
		for(size_t i = start; i != end; ++i) {
			s_cache_entry& centry = m_cache[ i ];
			if( !centry.reloadable )
				continue;

			centry.credits -= turns;
			if( centry.credits < 0 )
				unloadImage( centry );

		};
	};

}
;//FIFE
