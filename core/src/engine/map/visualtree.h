/***************************************************************************
 *   Copyright (C) 2006 by the FIFE Team                                   *
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

#ifndef FIFE_MAPVISUALTREE_H
#define FIFE_MAPVISUALTREE_H

// Standard C++ library includes
#include <vector>
#include <list>
#include <string>

// 3rd party library includes

// FIFE includes
#include "util/quadtree.h"

namespace FIFE { namespace map {

	class Visual;

	/** Internal helper class for View
	 */
	class VisualTree  {
		public:
			typedef std::vector<Visual*> RenderList;
			typedef std::vector<size_t> VisualRefList;

			/** Constructor
			 */
			VisualTree();

			/** Destructor
			 */
			~VisualTree();

			/** Add a visual to visual tree
			 *  \param visual Pointer to the visual
			 *  \return The ID which refers to the visual henceforth.
			 *  \note The VisualTree takes ownership of the object
			 */
			size_t addVisual(Visual* visual);

			/** Get a visual from the visual tree
			 * \param id The ID returned from a previous addVisual call
			 * \return The visual that was given in the addVisual call
			 * \see addVisual
			 */
			Visual* getVisual(size_t id);

			/** Get a visual from the visual tree
			 * \param id The ID of a visual
			 * \return true, if the id is valid
			 * \see getVisual
			 * \note This is only rarely useful (for example Camera::track)
			 */
			bool isValidVisualId(size_t id) const;

			/** Delete and remove the visual from the visual tree
			 *  \param id The ID returned from a previous addVisual call
			 */
			void removeVisual(size_t id);

			/** Notify the visual tree, that the visual has changed somehow
			 *  \param id The ID returned from a previous addVisual call
			 */
			void updateVisual(size_t id);

			/** Remove all visuals
			 */
			void clear();

			/** Calculate a list of visuals that have to be rendered
			 *  This will create a list of all visuals that intersect
			 *  with the viewport.
			 *  The returned list is in correct drawing order.
			 *  \param viewport The viewport in screen space
			 *  \return A list of visual IDs that have to be rendered
			 *
			 */
			RenderList& calculateRenderlist(const Rect& viewport);

			/** Find all visuals that intersect a given region
			 *  This calculates all visuals that intersect the given region.
			 *  \param region The region (in screen space)
			 *  \param reflist A list of visual IDs, that will be filled up.
			 */
			void findVisualsAt(const Rect& region, VisualRefList& reflist);


		private:
			typedef std::vector<size_t> type_vrefList;
			typedef struct {
				type_vrefList visuals;
			} s_vtype;
			typedef QuadTree<s_vtype,128> type_vtree;

			typedef struct {
				Visual *visual;
				type_vtree::Node *node;
			} s_visualEntry;
			typedef std::vector<s_visualEntry> type_visuals;
			typedef std::list<size_t> type_freevisuals;

			type_visuals m_visuals;
			type_freevisuals m_freevisuals;
			type_vtree m_vtree;

			RenderList m_renderlist;

			friend class MVT_RenderListBuilder;

			void removeVisualFromNode(type_vtree::Node *node,size_t id);
	};

} }

#endif //VISUALTREE
