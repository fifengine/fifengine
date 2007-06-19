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

// Standard C++ library includes
#include <algorithm>

// 3rd party library includes
#include <SDL.h>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "visual.h"
#include "visualtree.h"

#include "util/log.h"
#include "util/rect.h"
#include "map/structures/objectinfo.h"

namespace FIFE { namespace map {

	class MVT_RenderListBuilder {
		public:
			VisualTree::type_vrefList& renderlist;

			MVT_RenderListBuilder(VisualTree::type_vrefList& _renderlist)
				: renderlist(_renderlist) {
			}
			bool visit(VisualTree::type_vtree::Node* node, int d);
	};
	bool MVT_RenderListBuilder::visit(VisualTree::type_vtree::Node* node, int d) {
		VisualTree::type_vrefList &visuals = node->data().visuals;
		std::copy(visuals.begin(),visuals.end(),std::back_inserter(renderlist));
		return true;
	}

	VisualTree::VisualTree()
		: m_visuals(),
		m_freevisuals(),
		m_vtree() {
	}

	VisualTree::~VisualTree() {
		clear();
	}

	size_t VisualTree::addVisual(Visual *visual) {
		size_t visualId;

		assert( visual );

		if( !m_freevisuals.empty() ) {
			visualId = m_freevisuals.back();
			m_freevisuals.pop_back();
		} else {
			visualId = m_visuals.size();
			m_visuals.push_back( s_visualEntry() );
		}
		
		s_visualEntry &ventry = m_visuals[ visualId ];
		ventry.visual = visual;
		const Rect& screen_box = visual->getScreenBox();
		if( screen_box.w == 0 || screen_box.h == 0 ) {
// 			Log("vtree")
// 				<< "Discarding empty visual " << screen_box;
// 			if( visual->getObject() ) {
// 				visual->getObject()->debugPrint();
// 			}
			ventry.node = 0;
		} else {
			ventry.node = m_vtree.find_container( screen_box );
			ventry.node->data().visuals.push_back( visualId );
		}

		return visualId;
	}

	void VisualTree::removeVisual(size_t visualId) {
		s_visualEntry& ventry = m_visuals[ visualId ];
		assert( ventry.visual != 0 );

		if (ventry.node)
			removeVisualFromNode(ventry.node,visualId);

		delete ventry.visual;
		ventry.visual = 0;
		ventry.node = 0;

		m_freevisuals.push_back( visualId );
	}

	Visual* VisualTree::getVisual(size_t visualId) {
		assert( visualId < m_visuals.size() );

		s_visualEntry& ventry = m_visuals[ visualId ];
		assert( ventry.visual != 0 );

		return ventry.visual;
	}

	bool VisualTree::isValidVisualId(size_t visualId) const {
		if( visualId >= m_visuals.size() )
			return false;
		return m_visuals[ visualId ].visual != 0;
	}

	void VisualTree::clear() {
		for(size_t i=0; i!= m_visuals.size(); ++i) {
			delete m_visuals[i].visual;
		}
		m_visuals.clear();
		m_freevisuals.clear();
		m_vtree.clear();
	}

	void VisualTree::updateVisual(size_t visualId) {
		s_visualEntry& ventry = m_visuals[ visualId ];
		assert( ventry.visual != 0 );

		// Remove visualId from nodes visuals list
		if (ventry.node)
			removeVisualFromNode(ventry.node,visualId);

		// Add to new node
		const Rect& screen_box = ventry.visual->getScreenBox();
		if( screen_box.w == 0 || screen_box.h == 0 ) {
			ventry.node = 0;
			/*// Debug wrong screen boxes
			ObjectInfo* o = ventry.visual->getGObject();
			if (o && !o->isStatic) {
				o->debugPrint();
				Warn("map_visualtree")
					<< o->params["visual"] << ": "
					<< ventry.visual->getScreenBox()
					<< " not in vtree ";
			}*/
		} else {
			ventry.node = m_vtree.find_container( screen_box );
			ventry.node->data().visuals.push_back( visualId );
		}
	}

	bool visual_compare(Visual* a, Visual* b) {
		if( a->getLayerNumber() < b->getLayerNumber() )
			return true;
		if( a->getLayerNumber() > b->getLayerNumber() )
			return false;

		if( a->getZValue() == 0 && b->getZValue())
			return true;
		if( b->getZValue() == 0 && a->getZValue())
			return false;

		if( a->getLinearPosition() < b->getLinearPosition() )
			return true;
		if( a->getLinearPosition() > b->getLinearPosition() )
			return false;

		if( a->getZValue() < b->getZValue() )
			return true;
		if( a->getZValue() > b->getZValue() )
			return false;

		return reinterpret_cast<long>(a) < reinterpret_cast<long>(b);
	}


	VisualTree::RenderList& VisualTree::calculateRenderlist(const Rect& viewport) {
		type_vrefList vreflist;
		vreflist.reserve(2000);

		MVT_RenderListBuilder rlistbuilder(vreflist);

		type_vtree::Node *node = m_vtree.find_container( viewport );
		node->apply_visitor( rlistbuilder );

		node = node->parent();
		while( node ) {
			VisualTree::type_vrefList &visuals = node->data().visuals;
			std::copy(visuals.begin(),visuals.end(),std::back_inserter(vreflist));
			node = node->parent();
		}

		m_renderlist.clear();
		m_renderlist.reserve(vreflist.size());
		for(size_t i=0; i!= vreflist.size(); ++i) {
			Visual *visual = getVisual( vreflist[i] );
			const Rect& screen_box = visual->getScreenBox();

			if( viewport.intersects( screen_box ) ) {
				m_renderlist.push_back( visual );
			}

// 				ObjectInfo* o = visual->getGObject();
// 				if (o && !o->isStatic) {
// 					o->debugPrint();
// 					Warn("map_visualtree")
// 						<< o->params["visual"] << ": "
// 						<< visual->getScreenBox()
// 						<< " not in " << viewport;
// 				}
// 			}
		}
		std::sort(m_renderlist.begin(),m_renderlist.end(),visual_compare);

// 		Debug("map_visualtree") 
// 			<< "Renderlist contains " << m_renderlist.size() << " visuals"
// 			<< " in viewport " << viewport;

		return m_renderlist;
	}

	void VisualTree::findVisualsAt(const Rect& region, VisualTree::VisualRefList& reflist) {
		type_vrefList vreflist;
		MVT_RenderListBuilder rlistbuilder(vreflist);

		type_vtree::Node *node = m_vtree.find_container( region );
		node->apply_visitor( rlistbuilder );

		node = node->parent();
		while( node ) {
			VisualTree::type_vrefList &visuals = node->data().visuals;
			std::copy(visuals.begin(),visuals.end(),std::back_inserter(vreflist));
			node = node->parent();
		}

		for(size_t i=0; i!= vreflist.size(); ++i) {
			Visual *visual = getVisual( vreflist[i] );
			if( visual->getScreenBox().intersects( region ) ) {
				reflist.push_back( vreflist[i] );
			}
		}
	}

	void VisualTree::removeVisualFromNode(type_vtree::Node* node,size_t visualId) {
		type_vrefList &visuals = node->data().visuals;
		if( !visuals.empty() ) {
			for(size_t i = 0; i < visuals.size() - 1; ++i) {
				if( visuals[i] == visualId ) {
					visuals[i] = visuals[visuals.size()-1];
					break;
				}
			}
			visuals.pop_back();
		}
	}


} }
