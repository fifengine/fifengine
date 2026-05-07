// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

#ifndef FIFE_LAYER_H
#define FIFE_LAYER_H

// Platform specific includes
#include "platform.h"

// Standard C++ library includes
#include <algorithm>
#include <list>
#include <set>
#include <string>
#include <vector>

// 3rd party library includes

// FIFE includes
#include "instance.h"
#include "model/metamodel/modelcoords.h"
#include "model/metamodel/object.h"
#include "util/base/fifeclass.h"
#include "util/structures/rect.h"

namespace FIFE
{

    class Map;
    class CellGrid;
    class Object;
    class InstanceTree;
    class CellCache;
    class Trigger;

    /** Defines how pathing can be performed on this layer
     *
     * CELL_EDGES_ONLY allows pather to use only cell edges when moving instances from cell to cell on map
     * CELL_EDGES_AND_DIAGONALS allows pather to use both cell edges and diagonals when moving instances from cell to
     * cell on map
     */
    enum PathingStrategy : uint8_t
    {
        CELL_EDGES_ONLY,
        CELL_EDGES_AND_DIAGONALS
    };

    enum SortingStrategy : uint8_t
    {
        SORTING_CAMERA,
        SORTING_LOCATION,
        SORTING_CAMERA_AND_LOCATION
    };

    /** Listener interface for changes happening on a layer
     */
    class FIFE_API LayerChangeListener
    {
        public:
            virtual ~LayerChangeListener() = default;

            /** Called when some instance is changed on layer. @see InstanceChangeType
             * @param layer where change occurred
             * @param changedInstances list of instances containing some changes
             * @note Does not report creations and deletions
             */
            virtual void onLayerChanged(Layer* layer, std::vector<Instance*>& changedInstances) = 0;

            /** Called when some instance gets created on layer
             * @param layer where change occurred
             * @param instance which got created
             */
            virtual void onInstanceCreate(Layer* layer, Instance* instance) = 0;

            /** Called when some instance gets deleted on layer
             * @param layer where change occurred
             * @param instance which will be deleted
             * @note right after this call, instance actually gets deleted!
             */
            virtual void onInstanceDelete(Layer* layer, Instance* instance) = 0;
    };

    /** A basic layer on a map
     */
    class FIFE_API Layer : public FifeClass
    {
        public:
            /** Constructor
             * Layers are created by calling addLayer from map, thus
             * this method should really be called only by map or test code.
             */
            Layer(std::string identifier, Map* map, CellGrid* grid);

            /** Destructs a Layer instance
             */
            ~Layer();

            /** Get the id of this layer.
             */
            std::string const & getId() const;

            /** Sets the identifier for this layer.
             */
            void setId(std::string const & id);

            /** Get the map this layer is contained in
             */
            Map* getMap() const;

            /** Get the Cellgrid
             * @return pointer to a valid cellgrid
             */
            CellGrid* getCellGrid() const;

            /** Set the Cellgrid
             */
            void setCellGrid(CellGrid* grid);

            /** Get the instance tree.
             * @return this layers instance tree.
             */
            InstanceTree* getInstanceTree() const;

            /** Check existance of objects on this layer
             * @return True, if objects exist.
             */
            bool hasInstances() const;

            /** Add an instance of an object at a specific position
             */
            Instance* createInstance(Object* object, ModelCoordinate const & p, std::string const & id = "");

            /** Add an instance of an object at a specific position
             */
            Instance* createInstance(Object* object, ExactModelCoordinate const & p, std::string const & id = "");

            /** Add a valid instance at a specific position. This is temporary. It will be moved to a higher level
            later so that we can ensure that each Instance only lives in one layer.
             */
            bool addInstance(Instance* instance, ExactModelCoordinate const & p);

            /** Remove an instance from the layer
             */
            void removeInstance(Instance* instance);

            /** Remove an instance from the layer and delete it
             */
            void deleteInstance(Instance* instance);

            /** Get the list of instances on this layer
             */
            std::vector<Instance*> const & getInstances() const;

            /** Get the list of instances on this layer with the given identifier.
             */
            std::vector<Instance*> getInstances(std::string const & id);

            /** Returns instances that match given location.
             * @param loc location where to fetch instances from
             * @param use_exactcoordinates if true, comparison is done using exact coordinates. if not, cell coordinates
             * are used
             */
            std::vector<Instance*> getInstancesAt(Location& loc, bool use_exactcoordinates = false);

            /** Returns instances that match given rect.
             * @param rec rect where to fetch instances from
             */
            std::list<Instance*> getInstancesIn(Rect& rec);

            /** Returns instances that match given line between pt1 and pt2.
             * @param pt1 A const reference to the ModelCoordinate where to start from.
             * @param pt2 A const reference to the ModelCoordinate where the end is.
             * @return A vector that contain the instances.
             */
            std::vector<Instance*> getInstancesInLine(ModelCoordinate const & pt1, ModelCoordinate const & pt2);

            /** Returns instances that match given center and radius of the circle.
             * @param center A const reference to the ModelCoordinate where the center of the circle is.
             * @param radius A unsigned integer, radius of the circle.
             * @return A vector that contain the instances.
             */
            std::vector<Instance*> getInstancesInCircle(ModelCoordinate const & center, uint16_t radius);

            /** Returns all instances in the circle segment.
             * @param center A const reference to the ModelCoordinate where the center of the circle is.
             * @param radius A unsigned integer, radius of the circle.
             * @param sangle A interger, start angle of the segment.
             * @param eangle A interger, end angle of the segment.
             * @return A vector that contain the instances.
             */
            std::vector<Instance*> getInstancesInCircleSegment(
                ModelCoordinate const & center, uint16_t radius, int32_t sangle, int32_t eangle);

            /** Get the first instance on this layer with the given identifier.
             */
            Instance* getInstance(std::string const & identifier);

            /** Set object visibility
             */
            void setInstancesVisible(bool vis);

            /** Sets the transparency of all instances on the layer.  0=opaque, 255=transparent
             * @param transparency Transparency value from 0-255.
             */
            void setLayerTransparency(uint8_t transparency);

            /** Returns the layer's transparency value
             */
            uint8_t getLayerTransparency() const;

            /** Retrieves the minimum/maximum coordinates of instances on the layer.
             * @param min A reference to a ModelCoordinate that will hold the minimum coordinate.
             * @param max A reference to a ModelCoordinate that will hold the maximum coordinate.
             * @param layer A pointer to another layer that can be used to cast coordinates bettween layers.
             */
            void getMinMaxCoordinates(ModelCoordinate& min, ModelCoordinate& max, Layer const * layer = nullptr) const;

            /** Calculates z offset for the layer.
             * Is in range [-100,100], see glOrtho settings. Used by LayerCache to calculate z values.
             */
            float getZOffset() const;

            /** Get the overall number of layers.
             */
            uint32_t getLayerCount() const;

            /** Determines if a given cell on the layer contains a blocking instance
             *
             * @param cellCoordinate A const reference to a model coordinate of the cell in question.
             * @return A boolean, true if it is blocked false otherwise.
             */
            bool cellContainsBlockingInstance(ModelCoordinate const & cellCoordinate);

            /** Returns instances that blocks on given cell
             *
             * @param cellCoordinate A const reference to a model coordinate of the cell in question.
             * @return A vector that contains instances.
             */
            std::vector<Instance*> getBlockingInstances(ModelCoordinate const & cellCoordinate);

            /** Toggle object visibility
             * @see setObjectsVisible
             */
            void toggleInstancesVisible();

            /** Check object visibility
             * @see setObjectsVisible
             */
            bool areInstancesVisible() const;

            /** Called periodically to update events on layer
             * @returns true if layer was changed since the last update, false otherwise
             */
            bool update();

            /** Sets pathing strategy for the layer
             * @see PathingStrategy
             */
            void setPathingStrategy(PathingStrategy strategy);

            /** Gets pathing strategy for the layer
             * @see PathingStrategy
             */
            PathingStrategy getPathingStrategy() const;

            /** Sets sorting strategy for the layer
             * @see SortingStrategy
             */
            void setSortingStrategy(SortingStrategy strategy);

            /** Gets sorting strategy for the layer
             * @see SortingStrategy
             */
            SortingStrategy getSortingStrategy() const;

            /** Sets walkable for the layer. Only a walkable layer, can create a CellCache and
             *  only on a walkable, instances can move. Also interact layer can only be added to walkables.
             * @param walkable A boolean that mark a layer as walkable.
             */
            void setWalkable(bool walkable);

            /** Returns if a layer is walkable.
             * @return A boolean, true if the layer is walkable otherwise false.
             */
            bool isWalkable() const;

            /** Sets interact for the layer. The data(size, instances) from all interact layers
             *  and the walkable layer will merged into one CellCache.
             * @param interact A boolean that mark a layer as interact.
             * @param id A const reference to a string that should refer to the id of the walkable layer.
             */
            void setInteract(bool interact, std::string const & id);

            /** Returns if a layer is interact.
             * @return A boolean, true if the layer is interact otherwise false.
             */
            bool isInteract() const;

            /** Returns the id of the walkable layer if this is a interact layer otherwise the string is empty.
             * @return A const reference to a string that refer to the id of the walkable layer.
             */
            std::string const & getWalkableId();

            /** Adds a interact layer to the walkable layer.
             * @param layer A pointer to the interact layer that should be added.
             */
            void addInteractLayer(Layer* layer);

            /** Returns all assigned interact layer.
             * @return A const reference to a vector with pointers to interact layers.
             */
            std::vector<Layer*> const & getInteractLayers();

            /** Removes a interact layer from the walkable layer.
             * @param layer A pointer to the interact layer that should be removed.
             */
            void removeInteractLayer(Layer* layer);

            /** Called from Map to create a CellCache. Only walkable layers can create one CellCache.
             */
            void createCellCache();

            /** Returns the CellCache of this layer. In case there is no it returns NULL.
             * @return A pointer to the CellCache.
             */
            CellCache* getCellCache();

            /** Destroys the CellCache of this layer.
             */
            void destroyCellCache();

            /** Adds new change listener
             * @param listener to add
             */
            void addChangeListener(LayerChangeListener* listener);

            /** Removes associated change listener
             * @param listener to remove
             */
            void removeChangeListener(LayerChangeListener* listener);

            /** Returns true, if layer information was changed during previous update round
             */
            bool isChanged() const;

            /** Returns instances that were changed during previous update round.
             * @note does not contain created or deleted instances
             */
            std::vector<Instance*>& getChangedInstances();

            /** Sets the activity status for given instance on this layer.
             * @param instance A pointer to the Instance whose activity is to be changed.
             * @param active A boolean, true if the instance should be set active otherwise false.
             */
            void setInstanceActivityStatus(Instance* instance, bool active);

            /** Marks this layer as visual static. The result is that everything is rendered as one texture.
             *  If you have instances with actions/animations on this layer then they are not displayed correctly.
             * Note: Works currently only for OpenGL backend. SDL backend is restricted to the lowest layer.
             * @param stati A boolean, true if the layer should be static.
             */
            void setStatic(bool stati);

            /** Returns true, if layer is static.
             * @return A boolean, true if the layer is static, otherwise false.
             */
            bool isStatic() const;

        protected:
            //! string identifier
            std::string m_id;
            //! all the instances on this layer
            std::vector<Instance*> m_instances;
            //! all the active instances on this layer
            std::set<Instance*> m_activeInstances;
            //! walkable id
            std::string m_walkableId;
            //! all assigned interact layers
            std::vector<Layer*> m_interacts;
            //! listeners for layer changes
            std::vector<LayerChangeListener*> m_changeListeners;
            //! holds changed instances after each update
            std::vector<Instance*> m_changedInstances;
            //! pointer to map
            Map* m_map;
            //! The instance tree
            InstanceTree* m_instanceTree;
            //! layer's cellgrid
            CellGrid* m_grid;
            //! pointer to cellcache
            CellCache* m_cellCache;
            //! pathing strategy for the layer
            PathingStrategy m_pathingStrategy;
            //! sorting strategy for rendering
            SortingStrategy m_sortingStrategy;
            //! transparency, value 0 means total visible, 128 semi-transparent and 255 invisibility
            uint8_t m_transparency;
            //! if true the instances are visibility otherwise they are skipped during rendering
            bool m_instancesVisibility;
            //! is walkable true/false
            bool m_walkable;
            //! is interact true/false
            bool m_interact;
            //! true if layer (or it's instance) information was changed during previous update round
            bool m_changed;
            //! true if layer is static
            bool m_static;
    };

} // namespace FIFE

#endif
