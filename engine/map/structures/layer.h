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

#ifndef FIFE_MAP_LAYER_H
#define FIFE_MAP_LAYER_H

// Standard C++ library includes
#include <algorithm>
#include <map>
#include <string>
#include <vector>

// 3rd party library includes
#include <boost/variant.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/point.h"
#include "util/attributedclass.h"
#include "util/exception.h"

#include "objectinfo.h"

namespace FIFE { namespace map {

	class Layer;
	typedef boost::shared_ptr<Layer> LayerPtr;
	typedef boost::weak_ptr<Layer> LayerWeakPtr;

	class Elevation;
	typedef boost::shared_ptr<Elevation> ElevationPtr;
	typedef boost::weak_ptr<Elevation> ElevationWeakPtr;

	class Selection;
	typedef boost::shared_ptr<Selection> SelectionPtr;
	typedef boost::weak_ptr<Selection> SelectionWeakPtr;

	class Geometry;
	/** A basic layer on a map elevation
	 *
	 *  This class represents a layer on the Map.
	 *  This can be for example a Tile layer 
	 *  as the roofs and floors of a fo2 map
	 *  but can also just contain "objects"
	 *  on Layer coords.
	 * 
	 *  The tiles are *not* allways created only on
	 *  a first "setTileGID".
 	 *  
	 *  The most important features of this class are
	 *  "geometry", "shift" and "size":
	 *
	 *  The geometry is used to position objects on this
	 *  Layer and the Tiles too.
	 *
	 *  The shift is added to all screen coords and
	 *  will create the illusion of a height-difference :-)
	 *
	 *  The size simply is the maximum allowd size in Layer
	 *  coords this Layer covers.
	 *  
	 *  @bug The parameter code is untested, be warned.
	 *  @bug setTileGID and setParam behave differently on invalid positions.
	 *
	 *  Attributes: 
	 *  
	 *  Future:
	 *  	Connections between Layers to walk through (Elevators...)
	 *	Grouping of Layers (These Layers are roofs ... etc)
	 */
	class Layer : public AttributedClass {
		public:
			enum {
				MAX_PARAM = 100
			};

			/** Constructs a Layer instance
			 */
			static LayerPtr create(const Point& size, size_t geometry);

			/** Destructs a Layer instance
			 */
			~Layer();

			/** Get the elevation this layer is contained in
			 */
			ElevationPtr getElevation();

			/** Get the Geometry as set in the constructor
			 *  @return a valid Geometry
			 */
			Geometry* getGeometry();

			/** Get the size as set in the constructor
			 *  @return the size in Layer coords
			 */
			const Point& getSize() const;

			/** Set the screen coord shift
			 *  @param shift The new shift in screen coords
			 */
			void setShift(const Point& shift);

			/** Get number of layer in elevation
			 */
			size_t getLayerNumber() const;

			/** Get the screen coord shift
			 *  @return The shift in screen coords
			 */
			const Point& getShift() const;

			/** Check existance of objects on this layer
			 *  @return True, if objects exist.
			 */
			bool hasObjects() const;

			/** Add an object at a specific position
			 *  Does the equivalent to @c addObject( object, object->getPosition() )
			 */
			void addObject(ObjectPtr object);

			/** Add an object at a specific position
			 *  Does the equivalent to @c addObject( object, Point(x,y) )
			 */
			void addObjectAt(ObjectPtr object, int32_t x,int32_t y);

			/** Add an object at a specific position
			 */
			void addObjectAt(ObjectPtr object, const Point& p);

			/** Get a List of all objects at position
			 *  Does the equivalent of @c getObjectsAt( Point(x,y) )
			 */
			ObjectList& getObjectsAt(int32_t x,int32_t y);

			/** Get a List of all objects at position
			 *  Does the equivalent of @c getObjectsAt( Point(x,y) ) const
			 */
			const ObjectList& getObjectsAt(int32_t x,int32_t y) const;

			/** Get a List of all objects at position
			 */
			ObjectList& getObjectsAt(const Point& p);

			/** Get a List of all objects at position
			 */
			const ObjectList& getObjectsAt(const Point& p) const;

			/** Remove an object from the layer
			 */
			void removeObject(ObjectPtr object);

			/** Return a constant reference to the list of all objects on this layer.
			 */
			const ObjectList& getAllObjects() const;

			/** Apply a visitor to each object on this layer
			 */
			template<typename T>
			void forEachObject(T visitor) {
				std::for_each(m_all_objects.begin(),m_all_objects.end(),visitor);
			}

			/** Get the global id of the tile at the point
			 *  @note If the position is invalid or the Layer
			 *  is not yet created 0 is returned (a DummyImage id)
			 *  @param position A valid Layer position
			 *  @return A global id for the position
			 */
			size_t getTileGID(const Point&) const;

			/** Get the global id of the tile at the point
			 *  @see getGID
			 */
			size_t getTileGID(int32_t x, int32_t y) const;

			/** Set the global id of the tile at the point
			 *  @note If the position is invalid nothing is changed
			 *  @note if no tile have previously set, this
			 *  will allocate the needed memory.
			 *  @param position A valid Layer position
			 *  @param id A global id for the position
			 */
			void setTileGID(const Point& position, size_t id);

			/** Set the global id of the tile at the point
			 *  @see setTileGID
			 */
			void setTileGID(int32_t x, int32_t y, size_t id);

			/** Get the tile image id of a position
			 *  @note If the position is invalid or the Layer
			 *	is not yet created 0 is returned (a DummyImage id)
			 *  @param position A valid Layer position
			 *  @return An image id for the position
			 */
			size_t getTileImage(const Point&) const;

			/** Get the tile image id of a position
			 *  @see setTileImage
			 */
			size_t getTileImage(int32_t x,int32_t y) const;

			/** Set the tile image id of a position
			 *  @note If the position is invalid nothing is changed
			 *  @note If no tile have previously set, this
			 *  will allocate the needed memory.
			 *	@note This is exposed for LEGACY FALLOUT reasons;
			 *	use setTileGID instead to maintain current xml
			 *	map storage functionality.
			 *  @param position A valid Layer position
			 *  @param image_id An image id for the position
			 */
			void setTileImage(const Point& position, size_t image_id);

			/** Set the tile image id of a position
			 *  @see setTileImage
			 */
			void setTileImage(int32_t x,int32_t y, size_t image_id);

			/** Check existance of non-empty tiles
			 *  @return True, if tiles exist.
			 */
			bool hasTiles() const;

			/** Get a reference to a parameter Layer as vector
			 *  @param param_id The integer parameter ID
			 *  @return A reference to the Layer as @c std::vector
			 *  @throw std::out_of_range If the parameter ID is invalid.
			 *  @throw boost::bad_get If the parameter type @c T is invalid.
			 *
			 *  The @c std::vector is indexed like this:
			 *  @code
			 *  std::vector<T>& v = Layer->getParamLayer<T>( PARAM_ID ); 
			 *  T value = v[x + y * Layer->getSize().x ];
			 *  @endcode
			 * 
			 */
			template<typename T>
			std::vector<T>& getParamLayer(uint8_t param_id);
			
			/** Get a const reference to a parameter as vector
			 *  This is the @c const version of @see getParamLayer
			 */
			template<typename T>
			const std::vector<T>& getParamLayer(uint8_t param_id) const;
			
			/** Get a parameter reference at a Layer position
			 *  @param p The Layer position
			 *  @param param_id The integer parameter ID
			 *  @return A reference to the value at the Layer position
			 *  @throw std::out_of_range If the parameter ID is invalid.
			 *  @throw std::out_of_range If the Layer position is invalid.
			 *  @throw boost::bad_get If the parameter type @c T is invalid.
			 *  @warning Does not really check the position, so it might return a 
			 *  bogus reference for @c Point(-50,10) for example.
			 */
			template<typename T>
			T& getParam(const Point& p, uint8_t param_id);

			/** Get a parameter const reference at a Layer position
			 *  This is the @c const version of @see getParam
			 */
			template<typename T>
			const T& getParam(const Point& p, uint8_t type) const;

			/** Get a parameter reference at a Layer position
			 *  @see getParam
			 */
			template<typename T>
			T& getParam(int32_t x,int32_t y, uint8_t param_id);

			/** Get a parameter const reference at a Layer position
			 *  @see getParam
			 */
			template<typename T>
			const T& getParam(int32_t x,int32_t y, uint8_t type) const;

			/** Set a parameters value at a Layer position
			 *  @param p The Layer position
			 *  @param param_id The integer parameter ID
			 *  @param value The new value of the parameter
			 *  @throw std::out_of_range If the parameter ID is invalid.
			 *  @throw std::out_of_range If the Layer position is invalid.
			 *  @throw boost::bad_get If the parameter type @c T is invalid.
			 *  @warning Does not really check the position, so it might set a 
			 *  bogus value for @c Point(-50,10) for example.
			 */
			template<typename T>
			void setParam(const Point& p, uint8_t param_id, const T& value);

			/** Set a param value
			 *  This is a convenience version of @see setParam
			 */
			template<typename T>
			void setParam(int32_t x,int32_t y, uint8_t param_id, const T& value);

			/** Add a parameter by name and type
			 *  @param param_name String name of the new parameter
			 *  @param _default Default value of the parameter
			 *  @return Integer param ID of the new parameter
			 *  @throw NameClash If the string parameter name already is in use
			 *  @throw NotSupported If too many parameters are already in use (250)
			 */
			template<typename T>
			uint8_t addParam(const std::string& param_name, const T& _default = T());

			/** Remove a parameter by id
			 *  @param param_id Integer id
			 *  @throw std::out_of_range If the parameter ID is invalid.
			 */
			void removeParam(uint8_t param_id);

			/** Get Integer parameter id by parameter name
			 *  @param param_name String name of a parameter
			 *  @return Integer parameter ID
			 *  @throw NotFound if the string parameter name does not exist.
			 */
			uint8_t getParamByName(const std::string& param_name) const;

			/** Check existence of a parameter
			 *  @param param_name String name of a parameter.
			 *  @return True, if the parameter exists.
			 */
			bool hasParamByName(const std::string& param_name) const;

			/** Check existence of a parameter
			 *  @param param_id Integer ID of a parameter.
			 *  @return True, if the parameter exists.
			 */
			bool hasParam(uint8_t param_id) const;

			/** Set Tiles visible
			 */
			void setTilesVisible(bool vis);

			/** Toggle tile visibility
			 * @see setTilesVisible
			 */
			void toggleTilesVisible();

			/** Check tile visibility
			 * @see setTilesVisible
			 */
			bool areTilesVisible() const;

			/** Set object visibility
			 */
			void setObjectsVisible(bool vis);

			/** Toggle object visibility
			 *  @see setObjectsVisible
			 */
			void toggleObjectsVisible();

			/** Check object visibility
			 *  @see setObjectsVisible
			 */
			bool areObjectsVisible() const;

			/** Set global tile alpha value
			 *  All tiles are rendered with this alpha value.
			 */
			void setGlobalAlpha(uint8_t);

			/** Get global tile alpha value
			 *  @see setGlobalAlpha
			 */
			uint8_t getGlobalAlpha() const;

			/** Change global tile alpha value
			 *  Ensures no overflow happens and the alpha
			 *  value stays between 0 (transparent) and
			 *  255(opaque)
			 *  @see setGlobalAlpha
			 */
			void addToGlobalAlpha(int delta);

			/** Check whether the grid lines shall be drawn
			 *  Grid lines are drawn by setting an overlay
			 *  image, which is drawn over each tiles position
			 */
			bool isGridOverlayEnabled() const;
			void setGridOverlayEnabled(bool e);
			void toggleGridOverlayEnabled();

			/** Check whether a position is on the Layer
			 *  @param positon A Layer position to check
			 *  @return True, only if the position is on the Layer
			 */
			bool isValidPosition(const Point& pos) const;

			/** Check whether a position is on the Layer
			 *  @see isValidPosition
			 */
			bool isValidPosition(int32_t x,int32_t y) const;

			SelectionPtr getSelection() const;

			/** Get total number of layers
			 */
			static long globalCount();
		protected:
			/** Constructs a Layer instance
			 */
			Layer(const Point& size, size_t geometry);
			void insertedObject(ObjectPtr object, const Point& p);

			LayerWeakPtr m_self;
			ElevationWeakPtr m_elevation;
			static long m_count;

			Point m_size;

			uint8_t m_global_alpha;
			size_t m_layer_num;

			bool m_tiles_visibility;
			bool m_objects_visibility;
			bool m_grid_overlay;

			// NOTE: is the expense of storing both tiles and tilegids worth
			// computation saved from not calculating tiles from tilegids each
			// time?
			std::vector<size_t> m_tilegids;
			std::vector<size_t> m_tiles;

			ObjectList m_all_objects;
			std::vector<ObjectList> m_objects;

			Point m_shift;
			size_t m_geometry_id;
			Geometry* m_geometry;
			mutable SelectionPtr m_selection;

			typedef boost::variant<
				std::vector<long>,
				std::vector<bool>,
				std::vector<float>
			> type_varLayer;

			std::vector<type_varLayer> m_paramLayers;
			typedef std::map<std::string,uint8_t> type_paramnames;
			typedef std::map<uint8_t,type_paramnames::iterator> type_paramnames_reverse;
			type_paramnames m_paramnames;
			type_paramnames_reverse m_paramnames_reverse;

			std::vector<uint8_t> m_param_freelist;
			// For setting layernum
			friend class Elevation;
	};

	inline
	bool Layer::isValidPosition(const Point& p) const {
		return p.x >= 0 && p.x < m_size.x && p.y >= 0 && p.y < m_size.y;
	}

	inline
	bool Layer::isValidPosition(int32_t x, int32_t y) const {
		return x >= 0 && x < m_size.x && y >= 0 && y < m_size.y;
	}

	inline 
	size_t Layer::getLayerNumber() const {
		return m_layer_num;
	}

	inline
	size_t Layer::getTileGID(const Point& p) const {
		if (!isValidPosition(p.x,p.y) || m_tilegids.empty()) {
			return 0;
		}
		return m_tilegids[p.x + p.y * m_size.x];
	}

	inline
	size_t Layer::getTileGID(int32_t x, int32_t y) const {
		return getTileGID(Point(x,y));
	}

	inline
	size_t Layer::getTileImage(int32_t x, int32_t y) const {
		if (!isValidPosition(x,y) || m_tiles.empty()) {
			return 0;
		}
		return m_tiles[x + y * m_size.x];
	}

	inline
	size_t Layer::getTileImage(const Point& p) const {
		if (!isValidPosition(p) || m_tiles.empty()) {
			return 0;
		}
		return m_tiles[p.x + p.y * m_size.x];
	}

	template<typename T>
	inline
	std::vector<T>& Layer::getParamLayer(uint8_t param_id) {
		return boost::get<T>(m_paramLayers.at(param_id));
	}
	
	template<typename T>
	inline
	const std::vector<T>& Layer::getParamLayer(uint8_t param_id) const {
		return boost::get<T>(m_paramLayers.at(param_id));
	}
	
	template<typename T>
	inline
	T& Layer::getParam(const Point& p, uint8_t param_id)  {
		return boost::get<T>(m_paramLayers.at(param_id)).at(p.x + p.y * m_size.x);
	}

	template<typename T>
	inline
	const T& Layer::getParam(const Point& p, uint8_t param_id) const {
		return boost::get<T>(m_paramLayers.at(param_id)).at(p.x + p.y * m_size.x);
	}

	template<typename T>
	inline
	T& Layer::getParam(int32_t x,int32_t y, uint8_t param_id)  {
		return boost::get<T>(m_paramLayers.at(param_id)).at(x + y * m_size.x);
	}

	template<typename T>
	inline
	const T& Layer::getParam(int32_t x,int32_t y, uint8_t param_id) const {
		return boost::get<T>(m_paramLayers.at(param_id)).at(x + y * m_size.x);
	}

	template<typename T>
	inline
	void Layer::setParam(const Point& p, uint8_t param_id, const T& value) {
		boost::get<T>(m_paramLayers.at(param_id)).at(p.x + p.y * m_size.x) = value;
	}

	template<typename T>
	inline
	void Layer::setParam(int32_t x,int32_t y, uint8_t type, const T& value) {
		boost::get<T>(m_paramLayers.at(type)).at(x + y * m_size.x) = value;
	}

	template<typename T>
	inline
	uint8_t Layer::addParam(const std::string& param_name, const T& _default) {

		if( m_paramnames.find(param_name) != m_paramnames.end() ) {
			throw NameClash(param_name);
		}

		if( m_paramLayers.size() == MAX_PARAM ) {
			throw NotSupported("More than Layer::MAX_PARAM parameters per Layer.");
		}

		uint8_t param_id;
		if( !m_param_freelist.empty() ) {
			param_id = m_param_freelist.back();
			m_param_freelist.pop_back();
			m_paramLayers[ param_id ] = std::vector<T>();
		} else {
			param_id = m_paramLayers.size();
			m_paramLayers.push_back( std::vector<T>() );
		}

		boost::get<std::vector<T> >(m_paramLayers[ param_id ])
			.resize(m_size.x*m_size.y, _default);
		m_paramnames[ param_name ] = param_id;
		m_paramnames_reverse[ param_id ] = m_paramnames.find(param_name);
		return param_id;
	}

	inline
	uint8_t Layer::getParamByName(const std::string& param_name) const {
		type_paramnames::const_iterator i = m_paramnames.find(param_name);
		if( i == m_paramnames.end() ) {
			throw NotFound(param_name);
		}
		return i->second;
	}

	inline
	bool Layer::hasParamByName(const std::string& param_name) const {
		return m_paramnames.find(param_name) != m_paramnames.end();
	}

	inline
	bool Layer::hasParam(uint8_t param_id) const {
		return m_paramLayers.size() > param_id;
	}


} } // FIFE::map

#endif
