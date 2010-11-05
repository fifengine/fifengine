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

#ifndef FIFE_VIEW_CAMERA_H
#define FIFE_VIEW_CAMERA_H

// Standard C++ library includes
#include <string>
#include <map>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "util/structures/rect.h"
#include "util/math/matrix.h"

#include "rendererbase.h"

namespace FIFE {

	typedef Point3D ScreenPoint;
	class Layer;
	class Instance;
	class ImagePool;
	class AnimationPool;
	class RenderBackend;
	class LayerCache;
	class MapObserver;
	typedef std::map<Layer*, RenderList > t_layer_to_instances;

	/** Camera describes properties of a view port shown in the main screen
	 *  Main screen can have multiple cameras active simultanously
	 *  Different cameras can have different properties, like location
	 *  to shoot, zoom or tilt
	 */
	class Camera: public IRendererListener, public IRendererContainer {
	public:
		enum Transform {
			NormalTransform = 0,
			WarpedTransform = 1
		};

		/** Constructor
		 * Camera needs to be added to the view. If not done so, it is not rendered.
		 * @param id identifier for the camera
		 * @param layer layer where camera is bound. Camera is bound to a layer for two reasons:
		 *   * camera's scaling is done based on cell image dimensions. Cell image is layer based (@see setCellImageDimensions)
		 *   * camera could be bound to a pather, which operates on layer
		 * @param viewport used viewport for the camera. Viewport is measured in pixels in relation to game main screen
		 * @param emc coordinate, where camera is focused on given layer
		 * @param renderbackend to use with rendering
		 * @param ipool to use with rendering
		 * @param apool to use with rendering
		 */
		Camera(const std::string& id,
			Layer* layer,
			const Rect& viewport,
			RenderBackend* renderbackend,
			ImagePool* ipool,
			AnimationPool* apool);

		/** Destructor
		 */
		virtual ~Camera();

		/** Gets the identifier for this camera.
		 */
		const std::string& getId() const { return m_id; }

		/** Sets the identifier for this camera.
		 */
		void setId(const std::string& id) { m_id = id; }

		/** Sets tilt for the camera.
		 * e.g. overhead camera has tilt 0, while traditional isometric camera has tilt 45
		 * @param tilt tilt for the camera
		 */
		void setTilt(double tilt);

		/** Gets camera tilt
		 * @return tilt of camera
		 */
		double getTilt() const;

		/** Sets rotation for the camera.
		 * Rotation can be visualized by thinking camera that rotates around an object
		 * that it is rendering
		 * @param rotation rotation for the camera
		 */
		void setRotation(double rotation);

		/** Gets camera rotation
		 * @return rotation of the camera
		 */
		double getRotation() const;

		/** Sets zoom for the camera.
		 * @param zoom zoom for the camera
		 */
		void setZoom(double zoom);

		/** Gets camera zoom
		 * @return zoom of the camera
		 */
		double getZoom() const;

		/** Sets screen cell image dimensions.
		 * Cell image dimension is basically width and height of a bitmap, that covers
		 * one cell in the layer where camera is bind
		 * @return Point Point containing x=width and y=height
		 */
		void setCellImageDimensions(unsigned int width, unsigned int height);

		/** Gets screen cell image dimensions.
		 * @see setCellImageDimensions
		 * @return Point containing x=width and y=height
		 */
		Point getCellImageDimensions();

		/** Gets screen cell image dimensions for given layer.
		* @return Point Point containing x=width and y=height
		*/
		Point getCellImageDimensions(Layer* layer);

		/** Sets the location for camera
		 * @param location location (center point) to render
		 */
		void setLocation(const Location& location);

		/** Gets the location camera is rendering
		 * @return camera location
		 */
		Location getLocation() const;

		Point3D getOrigin() const;

		/** Gets a reference to the camera location
		 * @note if you change returned location without calling Camera::setLocation(...),
		 *       remember to call Camera::refresh() (otherwise camera transforms are not updated)
		 * @return reference to the camera location
		 */
		Location& getLocationRef();

		/** Attaches the camera to an instance.
		 * @param instance Instance to which the camera shall be attached
		 * @note The camera can only be attached to an instance at the same layer!
		 */
		void attach(Instance *instance);

		/** Detaches the camera from an instance.
		 */
		void detach();

		/** Returns instance where camera is attached. NULL if not attached
		 */
		Instance* getAttached() const { return m_attachedto; }

		/** Sets the viewport for camera
		 * viewport is rectangle inside the view where camera renders
		 * @param viewport area for camera render
		 */
		void setViewPort(const Rect& viewport);

		/** Gets the viewport for camera
		 * @return camera viewport
		 */
		const Rect& getViewPort() const;

		/** Transforms given point from screen coordinates to map coordinates
		 * @param screen_coords screen coordinates to transform
		 * @param z_calculated if true, z-value (depth cut point) is pre-calculated. If false, camera calculates it
		 * @return point in map coordinates
		 */
		ExactModelCoordinate toMapCoordinates(ScreenPoint screen_coords, bool z_calculated=true);

		/** Transforms given point from map coordinates to screen coordinates
		 *  @return point in screen coordinates
		 */
		ScreenPoint toScreenCoordinates(ExactModelCoordinate map_coords);

		/** Transforms given point from map coordinates to virtual screen coordinates
		 *  @return point in screen coordinates
		 */
		DoublePoint3D toVirtualScreenCoordinates(ExactModelCoordinate map_coords);

		ScreenPoint virtualScreenToScreen(const DoublePoint3D& p);
		DoublePoint3D screenToVirtualScreen(const ScreenPoint& p);

		/** Sets camera enabled / disabled
		 */
		void setEnabled(bool enabled);

		/** Gets if camera is enabled / disabled
		 */
		bool isEnabled();

		/** Returns instances that match given screen coordinate
		 * @param screen_coords screen coordinates to be used for hit search
		 * @param layer layer to use for search
		 * @param instances list of instances that is filled based on hit test results
		 */
		void getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances);

		/** Returns instances that match given screen coordinate
		 * @param screen_point1 top left screen coordinates to be used for hit search
		 * @param screen_point2 right bottom screen coordinates to be used for hit search
		 * @param layer layer to use for search
		 * @param instances list of instances that is filled based on hit test results
		 */
		void getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances);

		/** Returns instances that match given location. Instances are sorted based on camera view, so that "topmost"
		 * instance is first in returned list
		 * @param loc location where to fetch instances from
		 * @param instances list of instances that is filled based on hit test results
		 * @param use_exactcoordinates if true, comparison is done using exact coordinates. if not, cell coordinates are used
		 */
		void getMatchingInstances(Location& loc, std::list<Instance*>& instances, bool use_exactcoordinates=false);

		/** General update routine.
		 * In this function, the camera's position gets updated when its attached
		 * to another instance.
		 * @note call this only once in engine update cycle, so that tracking between
		 *  current position and previous position keeps in sync. This information
		 *  is used e.g. by view to fix the pixel wobbling problem
		 */
		void update();

		/** Refreshes camera view in case e.g. location is updated directly (not via setLocation)
		 * @note calling this function marks camera as "warped", therefore it causes all instance
		 * positions to be recalculated. If you constantly call this, you end up with pixel wobbling
		 * effect when camera is moved.
		 */
		void refresh();

		/** Resets temporary values from last update round, like warped flag
		 */
		void resetUpdates();

		/** Adds new renderer on the view. Ownership is transferred to the camera.
		 */
		void addRenderer(RendererBase* renderer);

		/** Gets renderer with given name
		 */
		RendererBase* getRenderer(const std::string& name);

		/** resets active layer information on all renderers.
		 */
		void resetRenderers();

		/** calculates z-value for given screenpoint
		 */
		void calculateZValue(ScreenPoint& screen_coords);

		void onRendererPipelinePositionChanged(RendererBase* renderer);
		void onRendererEnabledChanged(RendererBase* renderer);

		/** Use with OpenGL only! Checks to see if the entire viewport was rendered last frame.
		 * If so it wont clear the backbuffer.
		 */
		bool testRenderedViewPort();

		void setLightingColor(float red, float green, float blue, float alpha);
		void resetLightingColor();
		std::vector<float> getLightingColor();

		/** Renders camera
		 */
		void render();

	private:
		friend class MapObserver;
		void addLayer(Layer* layer);
		void removeLayer(Layer* layer);
		void updateMap(Map* map);
		std::string m_id;


		/** Updates the camera transformation matrix T with requested values.
		 * The requests are done using these functions :
		 * - setLocation
		 * - setRotation
		 * - setTilt
		 */
		void updateMatrices();

		/** Updates camera reference scale
		 * Reference scale is in a sense an internal zooming factor,
		 * which adjusts cell dimensions in logical space to ones shown on
		 * screen. Calculation is based on current camera properties (e.g. rotation)
		 * + given cell image dimensions for camera's layer
		 */
		void updateReferenceScale();

		/** Gets logical cell image dimensions for given layer
		 */
		DoublePoint getLogicalCellDimensions(Layer* layer);

		DoubleMatrix m_matrix;
		DoubleMatrix m_inverse_matrix;

		DoubleMatrix m_vs_matrix;
		DoubleMatrix m_vs_inverse_matrix;
		DoubleMatrix m_vscreen_2_screen;
		DoubleMatrix m_screen_2_vscreen;

		double m_tilt;
		double m_rotation;
		double m_zoom;
		Location m_location;
		ScreenPoint m_prev_origo;
		ScreenPoint m_cur_origo;
		Rect m_viewport;
		bool m_view_updated;
		unsigned int m_screen_cell_width;
		unsigned int m_screen_cell_height;
		double m_reference_scale;
		bool m_enabled;
		Instance* m_attachedto;
		bool m_backendSDL;
		// caches calculated image dimensions for already queried & calculated layers
		std::map<Layer*, Point> m_image_dimensions;
		bool m_iswarped;

		// list of renderers managed by the view
		std::map<std::string, RendererBase*> m_renderers;
		std::list<RendererBase*> m_pipeline;
		bool m_updated; // false, if view has never been updated before

		RenderBackend* m_renderbackend;
		ImagePool* m_ipool;
		AnimationPool* m_apool;

		// caches layer -> instances structure between renders e.g. to fast query of mouse picking order
		t_layer_to_instances m_layer_to_instances;

		std::map<Layer*,LayerCache*> m_cache;
		MapObserver* m_map_observer;
		Map* m_map;

		// is lighting enable
		bool m_lighting;
		// caches the light color for the camera
		std::vector<float> m_light_colors;
	};
}
#endif
