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
#include "view/renderers/instancerenderer.h"
%}

namespace FIFE {
	class Location;
	class RenderBackend;
	class ImagePool;
	class AnimationPool;

	class InstanceRenderer: public RendererBase {
	public:
		virtual ~InstanceRenderer();
		std::string getName();
		void addOutlined(Instance* instance, int r, int g, int b, int width);
		void removeOutlined(Instance* instance);
		void removeAllOutlines();
		void addColored(Instance* instance, int r, int g, int b);
		void removeColored(Instance* instance);
		void removeAllColored();
		void addTransparentArea(Instance* instance, const std::list<std::string> &groups, unsigned int w, unsigned int h, unsigned char trans, bool front = true);
		void removeTransparentArea(Instance* instance);
		void removeAllTransparentAreas();
		void addIgnoreLight(const std::list<std::string> &groups);
		void removeIgnoreLight(const std::list<std::string> &groups);
		void removeAllIgnoreLight();
		static InstanceRenderer* getInstance(IRendererContainer* cnt);
	private:
		InstanceRenderer(RenderBackend* renderbackend, int position, ImagePool* imagepool, AnimationPool* animpool);
	};
}
