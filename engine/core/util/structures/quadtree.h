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

#ifndef FIFE_UTIL_QUADTREE_H
#define FIFE_UTIL_QUADTREE_H

// Standard C++ library includes
#include <cassert>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "rect.h"

namespace FIFE {

/** QuadTree Node
 */
template<typename DataType, int32_t MinimumSize = 128>
class QuadNode {
	protected:
		QuadNode *m_parent;
		QuadNode *m_nodes[4];
		int32_t m_x,m_y,m_size;
		DataType m_data;

	public:
		/** Create a new QuadNode 
		 *  @param parent The parent QuadNode this node is contained in or null.
		 *  @param x The X position of this QuadNode.
		 *  @param y The Y position of this QuadNode.
		 *  @param size The width and height of this QuadNode.
		 */
		QuadNode(QuadNode* parent, int32_t x, int32_t y, int32_t size) 
			: m_parent(parent),m_x(x),m_y(y),m_size(size),m_data() {
			m_nodes[0] = m_nodes[1] = m_nodes[2] = m_nodes[3] = 0L;
		}

		~QuadNode() {
			delete m_nodes[0];
			delete m_nodes[1];
			delete m_nodes[2];
			delete m_nodes[3];
		}

		/** Find a container node for a given rectangle.
		 *  This guarantees to return a Node with the following
		 *  properties:
		 *  1.) The node contains the rectangle (as defined by the contains
		 *  function). 2.) All subnodes can not contain the rectangle or it has the MinimumSize.
		 *  3.) In case these properties can only be fulfilled by extending the tree upwards,
		 *  that is by creating a new root node - this function will return null.
		 *
		 *  This function will extend the tree automatically so that this guarantee
		 *  can be fulfilled.
		 */
		QuadNode* find_container(int32_t x, int32_t y, int32_t w, int32_t h);
		QuadNode* find_container(const Rect& rect) {
			return find_container(rect.x,rect.y,rect.w,rect.h);
		}

		/** Apply a visitor recursively to the QuadTree
		 * A visitor is an object which has a @c visit method which
		 * takes as parameters a pointer to a @c QuadNode and an integer.
		 * The integer is the depth of the given node.
		 * If the method returns @c true it is applied recursivly to all
		 * existing subnodes with a depth increased by one.
		 * The application happens in Z order (top left, top right, bottom
		 * left and finally bottom right).
		 */
		template<typename Visitor>
		void apply_visitor(Visitor& visitor, int32_t d = 0) {
			if( !visitor.visit(this, d) )
				return;
			if( m_nodes[0] ) m_nodes[0]->apply_visitor(visitor, d + 1);
			if( m_nodes[1] ) m_nodes[1]->apply_visitor(visitor, d + 1);
			if( m_nodes[2] ) m_nodes[2]->apply_visitor(visitor, d + 1);
			if( m_nodes[3] ) m_nodes[3]->apply_visitor(visitor, d + 1);
		}

		/** Return the X position of the node.
		 */
		int32_t x() const { return m_x; };

		/** Return the Y position of the node.
		 */
		int32_t y() const { return m_y; };

		/** Return the size (width and height) of the node.
		 */
		int32_t size() const { return m_size; };

		/** Return a reference to the data of the node.
		 */
		DataType& data() { return m_data; };
		
		/** Check whether a rectangle is contained in the node.
		 * A rectangle is contained in a node, iff:
		 * @code
		 *    x >= x() and x + w < x() + size() and y >= y() and y + h < y() + size()
		 * @endcode
		 * That is the top and left borders are inclusive, but the right and bottom
		 * borders are exclusive.
		 */
		bool contains(int32_t x, int32_t y, int32_t w, int32_t h) const;

		/// Expand the subnodes - only needed for debugging/profiling worst cases.
		void splice();

		/** Return the parent node
		 */
		QuadNode* parent() { return m_parent; };

		/** Create a new parent node for a rectangle
		 *  This will create a new parent node end expand the tree so that
		 *  the given rectangle will eventually be contained after enough calls
		 *  of this function.
		 */
		QuadNode* create_parent(int32_t x, int32_t y, int32_t w, int32_t h);
	protected:
		int32_t  subnode(int32_t x, int32_t y, int32_t w, int32_t h) const;
};

/** Dynamic QuadTree
 *  A space partitioning tree automatically expanding to adjust
 *  to any object size put into the data structure.
 */
template<typename DataType, int32_t MinimumSize = 128>
class QuadTree {
	public:
		typedef QuadNode<DataType,MinimumSize> Node;

		/** Create a new QuadTree
		 *  @param x The X position of the starting node.
		 *  @param y The Y position of the starting node.
		 *  @param starting_size The width and height of the starting node.
		 */
		QuadTree(int32_t x = 0, int32_t y = 0, int32_t starting_size = MinimumSize) {
			assert(starting_size>1);
			m_cursor = m_root = new Node(0L,x,y,starting_size);
		}

		~QuadTree() {
			assert( m_root->parent() == 0 );
			delete m_root;
		}

		/** Find a container node for a given rectangle.
		 *  This guarantees to return a Node with the following
		 *  properties:
		 *  1.) The node contains the rectangle (as defined by the contains
		 *  function). 2.) All subnodes can not contain the rectangle or it has the MinimumSize.
		 *  This function will extend the tree automatically so that this guarantee
		 *  can be fulfilled.
		 *  This function is optimized for sequential access. This means accessing different rectangles
		 *  that are 'near' to each other will be fast.
		 *  @warning If you put different sized objects in (for example) lists in the quadnode,
		 *  the returned node will @b not contain all objects which might intersect with the given
		 *  rectangle.
		 */
		Node* find_container(int32_t x, int32_t y, int32_t w, int32_t h);
		Node* find_container(const Rect& rect) {
			return find_container(rect.x,rect.y,rect.w,rect.h);
		}

		/** Apply a visitor recursively to the QuadTree
		 */
		template<typename Visitor>
		Visitor& apply_visitor(Visitor& visitor) {
			m_root->apply_visitor(visitor,0);
			return visitor;
		}

		void clear() {
			int32_t x = m_root->x();
			int32_t y = m_root->y();
			int32_t s = m_root->size();
			delete m_root;
			m_cursor = m_root = new Node(0L,x,y,s);
		}

	protected:
		Node *m_root;
		Node *m_cursor;
};


template<typename DataType, int32_t MinimumSize>
inline
bool QuadNode<DataType,MinimumSize>::contains(int32_t x, int32_t y, int32_t w, int32_t h) const {
	if (x < m_x)
		return false;
	if (y < m_y)
		return false;
	if (x + w >= m_x + m_size)
		return false;
	if (y + h >= m_y + m_size)
		return false;
	return true;
}

template<typename DataType, int32_t MinimumSize>
inline
int32_t QuadNode<DataType,MinimumSize>::subnode(int32_t x, int32_t y, int32_t w, int32_t h) const {
	/*
		Very small performance impact - roughly 5% for
		the already very fast find_container function.
	*/
	//assert(contains(x,y,w,h));

	if (x >= m_x + m_size/2) {
		if (y >= m_y + m_size/2) {
			return 3;
		}
		if (y + h < m_y + m_size/2) {
			return 1;
		}
		return -1;
	}
	if (x + w < m_x + m_size/2) {
		if (y >= m_y + m_size/2) {
			return 2;
		}
		if (y + h < m_y + m_size/2) {
			return 0;
		}
	}
	return -1;
}

template<typename DataType,int32_t MinimumSize>
QuadNode<DataType,MinimumSize>*
QuadNode<DataType,MinimumSize>::find_container(int32_t x, int32_t y, int32_t w, int32_t h) {
	if( !contains(x,y,w,h) ) {
		if (m_parent) {
			return m_parent->find_container(x,y,w,h);
		}
		return 0L;
	}

	if (m_size <= MinimumSize) {
		return this;
	}

	int32_t r = subnode(x,y,w,h);
	switch(r) {
	case -1:
		return this;
	case 0:
		if( m_nodes[0] == 0) {
			m_nodes[0] = new QuadNode<DataType,MinimumSize>(this,m_x,m_y,m_size/2);
		}
		return m_nodes[0]->find_container(x,y,w,h);
	case 1:
		if( m_nodes[1] == 0) {
			m_nodes[1] = new QuadNode<DataType,MinimumSize>(this,m_x + m_size/2,m_y,m_size/2);
		}
		return m_nodes[1]->find_container(x,y,w,h);
	case 2:
		if( m_nodes[2] == 0) {
			m_nodes[2] = new QuadNode<DataType,MinimumSize>(this,m_x,m_y + m_size/2,m_size/2);
		}
		return m_nodes[2]->find_container(x,y,w,h);
	case 3:
		if( m_nodes[3] == 0) {
			m_nodes[3] = new QuadNode<DataType,MinimumSize>(this,m_x + m_size/2,m_y + m_size/2,m_size/2);
		}
		return m_nodes[3]->find_container(x,y,w,h);
	default:
		assert("BUG in QuadTree !" == 0);
		return 0L;
	}
}

template<typename DataType,int32_t MinimumSize>
QuadNode<DataType,MinimumSize>* 
QuadNode<DataType,MinimumSize>::create_parent(int32_t x, int32_t y, int32_t w, int32_t h) {
	/*
		If used only by the tree, these two are superfluous.
	*/
	if( contains(x,y,w,h) )
		return this;
	if( m_parent )
		return m_parent;

	if (x >= m_x) {
		if (y >= m_y) { // we are node 0
			m_parent = new QuadNode(0L,m_x,m_y,m_size*2);
			m_parent->m_nodes[0] = this;
			return m_parent;
		}
		if (y + w < m_y + m_size) { // we are node 2
			m_parent = new QuadNode(0L,m_x,m_y - m_size,m_size*2);
			m_parent->m_nodes[2] = this;
			return m_parent;
		}
	}
	if (x + h < m_x + m_size) {
		if (y >= m_y) { // we are node 1
			m_parent = new QuadNode(0L,m_x-m_size,m_y,m_size*2);
			m_parent->m_nodes[1] = this;
			return m_parent;
		}
		if (y + w < m_y + m_size) { // we are node 3
			m_parent = new QuadNode(0L,m_x-m_size,m_y - m_size,m_size*2);
			m_parent->m_nodes[3] = this;
			return m_parent;
		}
	}

	// It does not matter....
	m_parent = new QuadNode(0L,m_x,m_y,m_size*2);
	m_parent->m_nodes[0] = this;
	return m_parent;
}

template<typename DataType,int32_t MinimumSize>
void QuadNode<DataType,MinimumSize>::splice() {
	if (m_size <= MinimumSize)
		return;

	if( m_nodes[0] == 0) {
		m_nodes[0] = new QuadNode<DataType,MinimumSize>(this,m_x,m_y,m_size/2);
	}
	if( m_nodes[1] == 0) {
		m_nodes[1] = new QuadNode<DataType,MinimumSize>(this,m_x + m_size/2,m_y,m_size/2);
	}
	if( m_nodes[2] == 0) {
		m_nodes[2] = new QuadNode<DataType,MinimumSize>(this,m_x,m_y + m_size/2,m_size/2);
	}
	if( m_nodes[3] == 0) {
		m_nodes[3] = new QuadNode<DataType,MinimumSize>(this,m_x + m_size/2,m_y + m_size/2,m_size/2);
	}
}

template<typename DataType,int32_t MinimumSize>
QuadNode<DataType,MinimumSize>*
QuadTree<DataType,MinimumSize>::find_container(int32_t x, int32_t y, int32_t w, int32_t h) {
	m_cursor = m_cursor->find_container(x,y,w,h);
	while( m_cursor == 0L ) {
		m_root = m_root->create_parent(x,y,w,h);
		m_cursor = m_root->find_container(x,y,w,h);
	}
	return m_cursor;
}


}

#endif // QUADTREE_H
