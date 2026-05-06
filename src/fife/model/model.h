// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_MODEL_H
#define FIFE_MODEL_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "model/metamodel/timeprovider.h"
#include "model/structures/map.h"
#include "util/base/fifeclass.h"

namespace FIFE
{

    class RenderBackend;
    class RendererBase;
    class MetaModel;
    class ModelMapObserver;
    class IPather;
    class Object;

    /**
     * A model is a facade for everything in the model.
     */
    class /*FIFE_API*/ Model : public FifeClass
    {
        public:
            /** Constructor
             *
             */
            Model(RenderBackend* renderbackend, std::vector<RendererBase*> const & renderers);

            /** Destructor
             *
             */
            ~Model();

            /** Add a map this model, and get a pointer to it.
             * The returned pointer is owned by the Model, so don't delete it!
             */
            Map* createMap(std::string const & identifier);

            /** Remove a map from this model
             */
            void deleteMap(Map*);

            /** Get all the maps in the model.
             */
            std::list<Map*> const & getMaps() const
            {
                return m_maps;
            }

            /** Get a map.
             *
             * @param identifier the id of the map to be found.
             */
            Map* getMap(std::string const & identifier) const;

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
            Object* createObject(
                std::string const & identifier, std::string const & name_space, Object* parent = nullptr);

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
            Object* getObject(std::string const & id, std::string const & name_space);

            /** Get all the objects in the given namespace.
             */
            std::list<Object*> getObjects(std::string const & name_space) const;

            /** Adds pather to model. Moves ownership to model
             */
            void adoptPather(IPather* pather);

            /** Returns pather corresponding given name. If none found, returns NULL
             */
            IPather* getPather(std::string const & pathername);

            /** Adds cellgrid to model. Moves ownership to model
             */
            void adoptCellGrid(CellGrid* grid);

            /** Returns new copy of cellgrid corresponding given name. If none found, returns NULL
             */
            CellGrid* getCellGrid(std::string const & gridtype);

            /** Removes cellgrid from model.
             */
            void removeCellGrid(CellGrid* grid);

            /** Called periodically to update events on model
             */
            void update();

            /** Sets speed for the model. With speed 1.0, everything runs with normal speed.
             * With speed 2.0, clock is ticking twice as fast. With 0, everything gets paused.
             * Negavtive values are not supported (throws NotSupported exception).
             */
            void setTimeMultiplier(float multip)
            {
                m_timeprovider.setMultiplier(multip);
            }

            /** Gets model speed. @see setTimeMultiplier.
             */
            double getTimeMultiplier() const
            {
                return m_timeprovider.getMultiplier();
            }

        private:
            // Map observer, currently only used to delete CellGrids from deleted layers
            ModelMapObserver* m_mapObserver;

            std::list<Map*> m_maps;

            using objectmap_t = std::map<std::string, Object*>;
            using namespace_t = std::pair<std::string, objectmap_t>;
            std::list<namespace_t> m_namespaces;

            /// Used to remember last 'selected' namespace.
            namespace_t* m_lastNamespace;

            /// Convenience function to retrieve a pointer to a namespace or NULL if it doesn't exist
            namespace_t* selectNamespace(std::string const & name_space);

            /// Convenience function to retrieve a pointer to a namespace or NULL if it doesn't exist
            namespace_t const * selectNamespace(std::string const & name_space) const;

            std::vector<IPather*> m_pathers;
            std::vector<CellGrid*> m_createdGrids;
            std::vector<CellGrid*> m_adoptedGrids;

            TimeProvider m_timeprovider;

            RenderBackend* m_renderbackend;

            std::vector<RendererBase*> m_renderers;
    };

}; // namespace FIFE
#endif
