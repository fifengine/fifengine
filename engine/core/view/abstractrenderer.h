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

#ifndef FIFE_ABSTRACTRENDERER_H
#define FIFE_ABSTRACTRENDERER_H

// Standard C++ library includes
#include <map>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE {
	class Camera;
	class Layer;
	class Instance;
	typedef std::map<int, std::vector<Instance*> > stackpos2instances_t;
	
	class AbstractRenderer {
	public:
		virtual ~AbstractRenderer() {};
		
		/** This method is called by the view to ask renderer to draw its rendering aspect based on
		 * given parameters. Renderers receive non-clipped instance stack since there is no
		 * way to know which information is relevant for the renderer. E.g. effect renderer
		 * might need to know offscreen instance locations to be able to draw radiation coming from
		 * some instance not visible on the screen.
		 *
		 * @param cam camera view to draw
		 * @param cam current layer to be rendered
		 * @param instance_stack instances on the current layer ordered by stack position
		 * @param stack_pos current stack position
		 */
		virtual void render(Camera* cam, Layer* layer, stackpos2instances_t* instance_stack, int stackpos) = 0;
	};
}

#endif
