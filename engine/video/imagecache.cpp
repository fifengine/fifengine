/***************************************************************************
 *   Copyright (C) 2005-2007 by the FIFE Team                              *
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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "loaders/video_loaders/complexanimation_provider.h"
#include "loaders/video_loaders/frm_provider.h"
#include "loaders/video_loaders/animation_provider.h"
#include "loaders/video_loaders/image_provider.h"
#include "loaders/video_loaders/subimage_provider.h"
#include "video/renderable.h"
#include "video/image.h"
#include "video/pixelbuffer.h"
#include "util/purge.h"

#include "util/debugutils.h"
#include "util/exception.h"
#include "video/imagecache.h"

namespace FIFE {

	class RenderableProviderConstructor {
		public:
			virtual ~RenderableProviderConstructor() {};
			virtual RenderableProviderPtr create(const RenderableLocation& location) = 0;
	};

	template<typename Klass, uint32_t allowed_types = RenderAble::RT_IMAGE|RenderAble::RT_UNDEFINED>
	class RenderableProviderConstructorTempl : public  RenderableProviderConstructor {
		public:
			virtual ~RenderableProviderConstructorTempl() {};
			virtual RenderableProviderPtr create(const RenderableLocation& location) {
				if( !(location.getType() & allowed_types) ) {
					return RenderableProviderPtr();
				}
				return RenderableProviderPtr(new Klass(location));
			};
	};

	typedef RenderableProviderConstructorTempl<
		video::loaders::FRMProvider,
		RenderAble::RT_IMAGE|RenderAble::RT_ANIMATION|RenderAble::RT_UNDEFINED
	> FRMConstructor;

	typedef RenderableProviderConstructorTempl<
		video::loaders::ComplexAnimationProvider,
		RenderAble::RT_COMPLEX_ANIMATION
	> CAPConstructor;

	typedef RenderableProviderConstructorTempl<
		video::loaders::SubImageProvider,
		RenderAble::RT_SUBIMAGE
	> SubImageConstructor;

	typedef RenderableProviderConstructorTempl<
		video::loaders::ImageProvider,
		RenderAble::RT_IMAGE|RenderAble::RT_UNDEFINED
	> IMGConstructor; 

	typedef RenderableProviderConstructorTempl<
		video::loaders::AnimationProvider,
		RenderAble::RT_ANIMATION
	> AnimationConstructor;

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
	static const size_t IC_MIN_ALIVE = 500;
	static const size_t IC_LOAD_CREDITS = 100;
	static const size_t IC_HIT_CREDITS = 1024;
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
		m_providerconstructors.push_front( new CAPConstructor() );
		m_providerconstructors.push_front( new SubImageConstructor() );
		m_providerconstructors.push_front( new AnimationConstructor() );
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
		purge( m_providerconstructors );
		m_providerconstructors.clear();
	}

	void ImageCache::printStatistics() {
		std::cout
			<< "Cache:   " << m_cache.size() << std::endl
			<< "Alive:   " << m_stats.alive << std::endl
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
		return addImageFromLocation( RenderableLocation( filename ) );
	};

	size_t ImageCache::addImageFromLocation( const RenderableLocation& location ) {
		if( !location.isValid() ) {
			Debug("image_cache") << "invalid location: " << location.toString();
			return UNDEFINED_ID;
		}

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
		centry.reloadable = provider || location.isValid();
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
		if (m_cache.size() <= index) {
			throw IndexOverflow( __FUNCTION__ );
		}

		s_cache_entry& centry = m_cache[ index ];
		if( centry.renderable ) {
			centry.credits = (IC_HIT_CREDITS + centry.credits*253)>>8;
			return centry.renderable;
		}
		return loadImage( index );
	}

	PixelBufferPtr ImageCache::getPixelBuffer(size_t image_id) {
		Image* image = dynamic_cast<Image*>(getImage(image_id));

		if( image == 0 ) {
			return PixelBufferPtr();
		}

		if( !image->getPixelBuffer() ) {
			unloadImage( image_id );
			image = dynamic_cast<Image*>(getImage(image_id));
		}

		if( image == 0 || !image->getPixelBuffer() ) {
			return PixelBufferPtr();
		}
		return image->getPixelBuffer();

	}

	RenderAble* ImageCache::loadImage( size_t index ) {
		s_cache_entry& centry = m_cache[ index ];
		if( centry.provider )
			centry.renderable = centry.provider->createRenderable();

		if (!centry.renderable && centry.location.isValid())
			findAndSetProvider(centry);

		if (!centry.renderable) {
			centry.renderable = new DummyImage(centry.location.toString());
			centry.provider = RenderableProviderPtr();
			centry.location = RenderableLocation();
			DEBUG_PRINT("Could not load: " << centry.location.toString());
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
		// Returns here, if centry.renderable stays zero.
	};

	void ImageCache::unloadImage( size_t index ) {
		if (m_cache.size() <= index) {
			throw IndexOverflow( __FUNCTION__ );
		}

		s_cache_entry& centry = m_cache[ index ];

		if (centry.reloadable)
			unloadImage( centry );
	}

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

			centry.credits -= 1;
			if( centry.credits < 0 )
				unloadImage( centry );

		};
	};

}
;//FIFE
