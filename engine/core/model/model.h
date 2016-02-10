/***************************************************************************
 *   Copyright (C) 2006-2011 by the FIFE team                              *
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

#ifndef FIFE_MODEL_H
#define FIFE_MODEL_H

// Standard C++ library includes
#include <list>
#include <map>
#include <vector>
#include <utility>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "util/base/fifeclass.h"

#include "model/structures/map.h"
#include "model/metamodel/timeprovider.h"

namespace FIFE {

	class RenderBackend;
	class RendererBase;
	class MetaModel;
	class IPather;
	class Object;

	/**
	 * A model is a facade for everything in the model.
	 */
	class Model: public FifeClass {
	public:

		/** Constructor
		 *
		 */
		Model(RenderBackend* renderbackend, const std::vector<RendererBase*>& renderers);

		/** Destructor
		 *
		 */
		~Model();

		/** Add a map this model, and get a pointer to it.
		 * The returned pointer is owned by the Model, so don't delete it!
		 */
		Map* createMap(const std::string& identifier);

		/** Remove a map from this model
		 */
		void deleteMap(Map*);

		/** Get all the maps in the model.
		 */
		const std::list<Map*>& getMaps() const { return m_maps; }

		/** Get a map.
		 *
		 * @param identifier the id of the map to be found.
		 */
		Map* getMap(const std::string& identifier) const;

		/** Return the number of maps in this model
		 */
		uint32_t getMapCount() const;

		/** Removes all maps from this model
		 */
		void deleteMaps();

		/** Return the number of enabled cameras in this model.
		 */
		uint32_t getActiveCameraCount() const;

		/** Get a list of namespaces currently referenced by objects in the metamodel.
		 */
		std::list<std::string> getNamespaces() const;

		/** Add an object to the metamodel.
		 *
		 * @param identifier A string for identifying this object; must be unique for its namespace.
		 * @param name_space A string identifying the namespace this object resides in.
		 * @param parent Objects may optionally inherit values from a parent object.
		 * @note This object belongs to the model, so don't delete the returned pointer
		 */
		Object* createObject(const std::string& identifier, const std::string& name_space, Object* parent=0);

		/** Attempt to remove an object from the model
		 *  Fails and returns false if the object is referenced by an instance.
		 */
		bool deleteObject(Object*);

		/** Attempt to remove all objects from the model
		 *  Fails and returns false if any maps with instances are present.
		 */
		bool deleteObjects();

		/** Get an object by its id. Returns 0 if object is not found.
		 */
		Object* getObject(const std::string& id, const std::string& name_space);

		/** Get all the objects in the given namespace.
		 */
		std::list<Object*> getObjects(const std::string& name_space) const;

		/** Adds pather to model. Moves ownership to model
		 */
		void adoptPather(IPather* pather);

		/** Returns pather corresponding given name. If none found, returns NULL
		 */
		IPather* getPather(const std::string& pathername);

		/** Adds cellgrid to model. Moves ownership to model
		 */
		void adoptCellGrid(CellGrid* grid);

		/** Returns new copy of cellgrid corresponding given name. If none found, returns NULL
		 */
		CellGrid* getCellGrid(const std::string& gridtype);

		/** Called periodically to update events on model
		 */
		void update();

		/** Sets speed for the model. With speed 1.0, everything runs with normal speed.
		 * With speed 2.0, clock is ticking twice as fast. With 0, everything gets paused.
		 * Negavtive values are not supported (throws NotSupported exception).
		 */
		void setTimeMultiplier(float multip) { m_timeprovider.setMultiplier(multip); }

		/** Gets model speed. @see setTimeMultiplier.
		 */
		double getTimeMultiplier() const { return m_timeprovider.getMultiplier(); }

	private:

		std::list<Map*> m_maps;

		typedef std::map<std::string,Object*> objectmap_t;
		typedef std::pair<std::string,objectmap_t> namespace_t;
		std::list<namespace_t> m_namespaces;

		/// Used to remember last 'selected' namespace.
		namespace_t* m_last_namespace;

		/// Convenience function to retrieve a pointer to a namespace or NULL if it doesn't exist
		namespace_t* selectNamespace(const std::string& name_space);

		/// Convenience function to retrieve a pointer to a namespace or NULL if it doesn't exist
		const namespace_t* selectNamespace(const std::string& name_space) const;

		std::vector<IPather*> m_pathers;
		std::vector<CellGrid*> m_created_grids;
		std::vector<CellGrid*> m_adopted_grids;
		//std::vector<CellGrid*> m_created_grids;
		TimeProvider m_timeprovider;

		RenderBackend* m_renderbackend;

		std::vector<RendererBase*> m_renderers;
	};

}; //FIFE
#endif
