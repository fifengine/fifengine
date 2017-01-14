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

#ifndef FIFE_VIEW_CAMERA_H
#define FIFE_VIEW_CAMERA_H

// Standard C++ library includes
#include <string>
#include <map>
#include <algorithm>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "model/structures/location.h"
#include "util/structures/rect.h"
#include "util/math/matrix.h"
#include "video/animation.h"

#include "rendererbase.h"

namespace FIFE {

	typedef Point3D ScreenPoint;
	class Layer;
	class Instance;
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
		enum TransformType {
			NoneTransform = 0x00,
			TiltTransform = 0x01,
			RotationTransform = 0x02,
			ZoomTransform = 0x04,
			PositionTransform = 0x08,
			ZTransform = 0x10
		};
		typedef uint32_t Transform;

		/** Constructor
		 * Camera needs to be added to the view. If not done so, it is not rendered.
		 * @param id identifier for the camera
		 * @param layer layer where camera is bound. Camera is bound to a layer for two reasons:
		 *   * camera's scaling is done based on cell image dimensions. Cell image is layer based (@see setCellImageDimensions)
		 *   * camera could be bound to a pather, which operates on layer
		 * @param viewport used viewport for the camera. Viewport is measured in pixels in relation to game main screen
		 * @param renderbackend to use with rendering
		 */
		Camera(const std::string& id,
			Layer* layer,
			const Rect& viewport,
			RenderBackend* renderbackend);

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

		/** Gets original zToY transformation value.
		 * @return zToY value of the camera.
		 */
		double getOriginalZToY() const;

		/** Sets zToY value for the camera and enables their use.
		 * This means the factor which influenced the z to y transformation,
		 * so if you set zToY=32 then 1z corresponds to 32 pixels in y direction.
		 * @param zToY influenced the z to y transformation of the camera.
		 */
		void setZToY(double zToY);

		/** Gets zToY value.
		 * @return zToY value of the camera.
		 */
		double getZToY() const;

		/** Sets z to y manipulation enabled / disabled.
		 * @param enabled If true then the zToY value is used instead of the original matrix value.
		 */
		void setZToYEnabled(bool enabled);

		/** Gets if z to y manipulation is enabled / disabled.
		 * @return true if z to y manipulation is enabled, otherwise false.
		 */
		bool isZToYEnabled() const;

		/** Sets screen cell image dimensions.
		 * Cell image dimension is basically width and height of a bitmap, that covers
		 * one cell in the layer where camera is bind
		 * @return Point Point containing x=width and y=height
		 */
		void setCellImageDimensions(uint32_t width, uint32_t height);

		/** Gets screen cell image dimensions.
		 * @see setCellImageDimensions
		 * @return Point containing x=width and y=height
		 */
		Point getCellImageDimensions();

		/** Gets screen cell image dimensions for given layer.
		* @return Point Point containing x=width and y=height
		*/
		Point getCellImageDimensions(Layer* layer);

		/** Gets x reference scale for cell image dimensions
		 */
		double getReferenceScaleX() const { return m_referenceScaleX; }

		/** Gets y reference scale for cell image dimensions
		 */
		double getReferenceScaleY() const { return m_referenceScaleY; }

		/** Gets a point that contain the visual z(z=1) difference, based on the given layer.
		 * @return Point3D Point3D containing x, y, z
		 */
		Point3D getZOffset(Layer* layer);

		/** Sets the location for camera
		 * @param location location (center point) to render
		 */
		void setLocation(const Location& location);

		/** Gets the location camera is rendering
		 * @return camera location
		 */
		Location getLocation() const;

		/** Gets screen point for the camera location
		 * @return camera screen point
		 */
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

		/** Gets the viewport for camera in pixel coordinates
		 * @return camera viewport
		 */
		const Rect& getViewPort() const;

		/** Gets the viewport for camera in map coordinates
		 * @return camera viewport
		 */
		const Rect& getMapViewPort();

		/** Gets the viewport for camera in layer coordinates
		 * @param layer A pointer to the layer whose geometry is used for the conversion
		 * @return camera viewport
		 */
		Rect getLayerViewPort(Layer* layer);

		/** Transforms given point from screen coordinates to map coordinates
		 * @param screen_coords screen coordinates to transform
		 * @param z_calculated if true, z-value (depth cut point) is pre-calculated. If false, camera calculates it
		 * @return point in map coordinates
		 */
		ExactModelCoordinate toMapCoordinates(ScreenPoint screen_coords, bool z_calculated=true);

		/** Transforms given point from map coordinates to screen coordinates
		 *  @return point in screen coordinates
		 */
		ScreenPoint toScreenCoordinates(const ExactModelCoordinate& map_coords);

		/** Transforms given point from map coordinates to virtual screen coordinates
		 *  @return point in virtual screen coordinates
		 */
		DoublePoint3D toVirtualScreenCoordinates(const ExactModelCoordinate& map_coords);

		/** Transforms given point from virtual screen coordinates to screen coordinates
		 *  @return point in screen coordinates
		 */
		ScreenPoint virtualScreenToScreen(const DoublePoint3D& p);

		/** Transforms given point from screen coordinates to virtual screen coordinates
		 *  @return point in virtual screen coordinates
		 */
		DoublePoint3D screenToVirtualScreen(const ScreenPoint& p);

		/** Sets camera enabled / disabled
		 */
		void setEnabled(bool enabled);

		/** Gets if camera is enabled / disabled
		 */
		bool isEnabled();

		/** Returns reference to RenderList.
		 */
		RenderList& getRenderListRef(Layer* layer);

		/** Returns instances that match given screen coordinate
		 * @param screen_coords screen coordinates to be used for hit search
		 * @param layer layer to use for search
		 * @param instances list of instances that is filled based on hit test results
		 * @param alpha the alpha to use to filter the matching instances.  Pixels
		 *        that have an alpha value higher than what is specified here are
		 *        considered a hit.
		 */
		void getMatchingInstances(ScreenPoint screen_coords, Layer& layer, std::list<Instance*>& instances, uint8_t alpha = 0);

		/** Returns instances that match given screen coordinate
		 * @param screen_rect rect that contains screen coordinates to be used for search
		 * @param layer layer to use for search
		 * @param instances list of instances that is filled based on hit test results
		 * @param alpha the alpha to use to filter the matching instances.  Pixels
		 *        that have an alpha value higher than what is specified here are
		 *        considered a hit.
		 */
		void getMatchingInstances(Rect screen_rect, Layer& layer, std::list<Instance*>& instances, uint8_t alpha = 0);

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

		/** Returns true if camera view has been updated, otherwise false
		 */
		bool isUpdated() { return m_updated; }

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

		/** Sets lighting color
		 */
		void setLightingColor(float red, float green, float blue);

		/** Resets lighting color
		 */
		void resetLightingColor();

		/** Returns a vector that contain the light color
		 */
		std::vector<float> getLightingColor();

		/** Sets a color as overlay
		 */
		void setOverlayColor(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha);

		/** Returns a vector that contain the overlay color
		 */
		std::vector<uint8_t> getOverlayColor();

		/** Resets the color overlay
		 */
		void resetOverlayColor();

		/** Sets a image as overlay,
		 *  if fill is true the image gets the viewport size.
		 */
		void setOverlayImage(int32_t id, bool fill = false);

		/** Returns the pool id of the overlay image
		 */
		int32_t getOverlayImage();

		/** Resets the image overlay
		 */
		void resetOverlayImage();

		/** Sets a animation as overlay,
		 *  if fill is true the animation gets the viewport size.
		 */
		void setOverlayAnimation(AnimationPtr anim, bool fill = false);

		/** Returns an AnimationPtr to the overlay animation
		 */
		AnimationPtr getOverlayAnimation();

		/** Resets the animation overlay
		 */
		void resetOverlayAnimation();

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

		/** Updates camera RenderLists
		 */
		void updateRenderLists();

		/** Gets logical cell image dimensions for given layer
		 */
		DoublePoint getLogicalCellDimensions(Layer* layer);

		/** Gets real cell image dimensions for given layer
		 */
		Point getRealCellDimensions(Layer* layer);

		/** Renders the overlay(color, image, animation) for the camera.
		 */
		void renderOverlay();

		/** Renders the layer part that is on screen as one image.
		 */
		void renderStaticLayer(Layer* layer, bool update);

		DoubleMatrix m_matrix;
		DoubleMatrix m_inverse_matrix;

		DoubleMatrix m_vs_matrix;
		DoubleMatrix m_vs_inverse_matrix;
		DoubleMatrix m_vscreen_2_screen;
		DoubleMatrix m_screen_2_vscreen;

		double m_tilt;
		double m_rotation;
		double m_zoom;
		double m_zToY;
		bool m_enabledZToY;
		Location m_location;
		ScreenPoint m_cur_origo;
		Rect m_viewport;
		Rect m_mapViewPort;
		bool m_mapViewPortUpdated;
		uint32_t m_screen_cell_width;
		uint32_t m_screen_cell_height;
		double m_referenceScaleX;
		double m_referenceScaleY;
		bool m_enabled;
		Instance* m_attachedto;
		// caches calculated image dimensions for already queried & calculated layers
		std::map<Layer*, Point> m_image_dimensions;
		// contains the geometry changes
		Transform m_transform;

		// list of renderers managed by the view
		std::map<std::string, RendererBase*> m_renderers;
		std::list<RendererBase*> m_pipeline;
		// false, if view has not been updated
		bool m_updated;

		RenderBackend* m_renderbackend;

		// caches layer -> instances structure between renders e.g. to fast query of mouse picking order
		t_layer_to_instances m_layerToInstances;

		std::map<Layer*,LayerCache*> m_cache;
		MapObserver* m_map_observer;
		Map* m_map;

		// is lighting enable
		bool m_lighting;
		// caches the light color for the camera
		std::vector<float> m_light_colors;

		// overlay stuff
		bool m_col_overlay;
		bool m_img_overlay;
		bool m_ani_overlay;
		SDL_Color m_overlay_color;
		int32_t m_img_id;
		AnimationPtr m_ani_ptr;
		bool m_img_fill;
		bool m_ani_fill;
		uint32_t m_start_time;
	};
}
#endif
