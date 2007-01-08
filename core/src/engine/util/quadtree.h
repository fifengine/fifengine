/***************************************************************************
 *   Copyright (C) 2006 by the Klaus Blindert                              *
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
#ifndef QUADTREE_H
#define QUADTREE_H
#include <cassert>

#include "video/rect.h"
namespace FIFE {

template<typename DataType, int MinimumSize = 128>
class QuadNode {
	protected:
		QuadNode *m_parent;
		QuadNode *m_nodes[4];
		int m_x,m_y,m_size;
		DataType m_data;

	public:
		QuadNode(QuadNode* parent, int x, int y, int size) 
			: m_parent(parent),m_x(x),m_y(y),m_size(size),m_data() {
			m_nodes[0] = m_nodes[1] = m_nodes[2] = m_nodes[3] = 0L;
		}

		~QuadNode() {
			delete m_nodes[0];
			delete m_nodes[1];
			delete m_nodes[2];
			delete m_nodes[3];
		}

		QuadNode* find_container(int x, int y, int w, int h);
		QuadNode* find_container(const Rect& rect) {
			return find_container(rect.x,rect.y,rect.w,rect.h);
		}

		template<typename Visitor>
		void apply_visitor(Visitor& visitor, int d = 0) {
			if( !visitor.visit(this, d) )
				return;
			if( m_nodes[0] ) m_nodes[0]->apply_visitor(visitor, d + 1);
			if( m_nodes[1] ) m_nodes[1]->apply_visitor(visitor, d + 1);
			if( m_nodes[2] ) m_nodes[2]->apply_visitor(visitor, d + 1);
			if( m_nodes[3] ) m_nodes[3]->apply_visitor(visitor, d + 1);
		}

		int x() const { return m_x; };
		int y() const { return m_y; };
		int size() const { return m_size; };
		DataType& data() { return m_data; };
		

		bool contains(int x, int y, int w, int h) const;

		// DEBUGGING
		void splice();
		QuadNode* parent() { return m_parent; };
		QuadNode* create_parent(int x, int y, int w, int h);
	protected:
		int  subnode(int x, int y, int w, int h) const;
};

template<typename DataType, int MinimumSize = 128>
class QuadTree {
	public:
		typedef QuadNode<DataType,MinimumSize> Node;

		QuadTree(int x = 0, int y = 0, int s = MinimumSize) {
			assert(s>1);
			m_cursor = m_root = new Node(0L,x,y,s);
		}
		~QuadTree() {
			assert( m_root->parent() == 0 );
			delete m_root;
		}

		Node* find_container(int x, int y, int w, int h);
		Node* find_container(const Rect& rect) {
			return find_container(rect.x,rect.y,rect.w,rect.h);
		}

		template<typename Visitor>
		Visitor& apply_visitor(Visitor& visitor) {
			m_root->apply_visitor(visitor,0);
			return visitor;
		}

		void clear() {
			int x = m_root->x();
			int y = m_root->y();
			int s = m_root->size();
			delete m_root;
			m_cursor = m_root = new Node(0L,x,y,s);
		}

	protected:
		Node *m_root;
		Node *m_cursor;
};


template<typename DataType, int MinimumSize>
inline
bool QuadNode<DataType,MinimumSize>::contains(int x, int y, int w, int h) const {
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

template<typename DataType, int MinimumSize>
inline
int QuadNode<DataType,MinimumSize>::subnode(int x, int y, int w, int h) const {
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

template<typename DataType,int MinimumSize>
QuadNode<DataType,MinimumSize>*
QuadNode<DataType,MinimumSize>::find_container(int x, int y, int w, int h) {
	if( !contains(x,y,w,h) ) {
		if (m_parent) {
			return m_parent->find_container(x,y,w,h);
		}
		return 0L;
	}

	if (m_size <= MinimumSize) {
		return this;
	}

	int r = subnode(x,y,w,h);
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

template<typename DataType,int MinimumSize>
QuadNode<DataType,MinimumSize>* 
QuadNode<DataType,MinimumSize>::create_parent(int x, int y, int w, int h) {
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

template<typename DataType,int MinimumSize>
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

template<typename DataType,int MinimumSize>
QuadNode<DataType,MinimumSize>*
QuadTree<DataType,MinimumSize>::find_container(int x, int y, int w, int h) {
	m_cursor = m_cursor->find_container(x,y,w,h);
	while( m_cursor == 0L ) {
		m_root = m_root->create_parent(x,y,w,h);
		m_cursor = m_root->find_container(x,y,w,h);
	}
	return m_cursor;
}


}

#endif // QUADTREE_H
