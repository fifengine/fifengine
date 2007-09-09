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

#ifndef FIFE_VIEW_VIEW_H
#define FIFE_VIEW_VIEW_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder


namespace FIFE {
	class Camera;
	class RenderBackend;
	class ImagePool;
	class AnimationPool;

	class View {
	public:
		/** Constructor
		 * @param renderbackend to use
		 * @param imagepool image pool where from fetch images
		 * @param animpool animation pool where from fetch images
		 */
		View(RenderBackend* renderbackend, ImagePool* imagepool, AnimationPool* animpool);

		/** Destructor
		 */
		~View();

		/** Adds new camera on view. Ownership is transferred to the view.
		 * After addition, camera gets rendered by the view
		 */
		void addCamera(Camera* camera);

		/** Removes given camera from view. Ownership is taken away from the view
		 */
		void removeCamera(Camera* camera);

		/** Causes view to render all cameras
		 */
		void update();

	private:
		void updateCamera(Camera* camera);
		int getAngleBetween(const Location& loc1, const Location& loc2, Camera& cam);
		
		// list of cameras managed by the view
		std::vector<Camera*> m_cameras;
		RenderBackend* m_renderbackend;
		ImagePool* m_imagepool;
		AnimationPool* m_animationpool;
	};

}
#endif
