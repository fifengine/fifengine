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

%module fife
%{
#include "view/view.h"
%}

namespace std {
  %template(CameraVector) std::vector<FIFE::Camera*>;
}

namespace FIFE {
	class RendererBase;
	class View {
	public:
		~View();
		Camera* addCamera(const std::string& id, Layer* layr,Rect viewport, ExactModelCoordinate emc);
		Camera* getCamera(const std::string& id);
		std::vector<Camera*>& getCameras();
		void removeCamera(Camera* camera);
		void clearCameras();
		void resetRenderers();
	private:
		View(RenderBackend* renderbackend);
	};
}
