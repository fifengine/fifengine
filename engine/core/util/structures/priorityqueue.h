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

#ifndef FIFE_SOLVER_INDEXEDPQ_H
#define FIFE_SOLVER_INDEXEDPQ_H

#include <cassert>
#include <list>

namespace FIFE {

	/** A pq which stores index-value pairs for elements.
	 *
	 * This acts as a normal PQ but stores some extra information about the
	 * elements that it's storing, namely a special unique index.
	 */
	template<typename index_type, typename priority_type>
	class PriorityQueue {
	public:
		/** Used for element ordering.
		 *
		 */
		enum Ordering {
			Ascending, //!< lowest priority first.
			Descending //!< highest priority first.
		};

		typedef std::pair<index_type, priority_type> value_type;

		/** Constructor
		 *
		 */
		PriorityQueue(void) : m_ordering(Ascending) {
		}

		/** Constructor
		 *
		 * @param ordering The ordering the priority queue should use.
		 */
		PriorityQueue(const Ordering ordering) : m_ordering(ordering) {
		}

		/** Pushes a new element onto the queue.
		 *
		 * The element is pushed onto the queue and then moved up the queue until it's
		 * in the correct position by priority.
		 *
		 * @param element Of type value_type which contains both the index and the priority of the element.
		 */
		void pushElement(const value_type& element);

		/** Pops the element with the highest priority from the queue.
		 *
		 * Removes and deletes the highest priority element.
		 */
		void popElement(void);

		/** Changes the priority of an element.
		 *
		 * Locates the element with the given index and changes it's priority to the given
		 * priority, it then re-orders the priority queue to take account of this new information.
		 *
		 * @param index The index of the element to change the priority of.
		 * @param newPriority The new priority of the element.
		 * @return True if the element could be found, false otherwise.
		 */
		bool changeElementPriority(const index_type& index, const priority_type& newPriority);

		/** Removes all elements from the priority queue.
		 *
		 */
		void clear(void);

		/** Retrieves the element with the highest priority.
		 *
		 * This function will generate an assertion error if the pq is
		 * empty.
		 *
		 * @return A const reference to the highest priority element.
		 */
		const value_type getPriorityElement(void) const {

			assert(!empty());

			return m_elements.front();

		}

		/** Determines whether the queue is currently empty.
		 *
		 * @return true if it is empty, false otherwise.
		 */
		bool empty(void) const {
			return m_elements.empty();
		}

		/** Returns the current size of the queue.
		 *
		 */
		size_t size(void) const {
			return m_elements.size();
		}
	private:
		typedef std::list<value_type> ElementList;
		typedef typename ElementList::iterator ElementListIt;
		typedef typename ElementList::const_iterator ElementListConstIt;

		//A list of valuetype pairs that represents the pq.
		ElementList m_elements;

		//The order to use when sorting the pq.
		Ordering    m_ordering;

		/** Orders a PQ element up the list.
		 *
		 * @param i An iterator representing the element in the list to be sorted up.
		 */
		void orderUp(ElementListIt i);
		/** Orders a PQ element up the list.
		 *
		 * @param entry A const reference to a value_type which represents the element to be added to the
		 * pq.
		 */
		void orderUp(const value_type& entry);

		/** Orders a PQ element down the list.
		 *
		 * @param i An iterator representing the element in the PQ to order down.
		 */
		void orderDown(ElementListIt i);

		/** Retrieves the iterator to the element with the given index.
		 *
		 * @param index A const reference to the index to find.
		 */
		ElementListIt getElementIterator(const index_type& index) {

			for(ElementListIt i = m_elements.begin(); i != m_elements.end(); ++i) {
				if(i->first == index) {
					return i;
				}
			}

			return m_elements.end();

		}

		/** The comparison function, used to compare two elements.
		 *
		 * @param a The l-operand of the comparison operation.
		 * @param b The r-operand of the comparison operation.
		 * @return An integer representing the result of the comparison operation. 1 being a is greather than b,
		 *		   -1 being a is less than b and 0 meaning that they're equal.
		 */
		int32_t compare(const value_type& a, const value_type& b);
	};
}

template<typename index_type, typename priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::pushElement(const value_type& element) {
	if(empty()) {
		m_elements.push_front(element);
	}
	else {
		orderUp(element);
	}
}

template<typename index_type, typename priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::popElement(void) {

	if(!empty()) {
		m_elements.pop_front();
	}

}

template<typename index_type, typename priority_type>
bool FIFE::PriorityQueue<index_type, priority_type>::changeElementPriority(const index_type& index, const priority_type& newPriority) {

	ElementListIt i = getElementIterator(index);

	if(i == m_elements.end()) {
		return false;
	}

	int32_t compare_res = compare(value_type(index, newPriority), (*i));

	i->second = newPriority;

	if(compare_res > 0 &&  i != m_elements.begin()) {
		orderDown(i);
	} else if(compare_res < 0) {
		orderUp(i);
	}

	return true;

}

template<typename index_type, typename priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::clear(void) {

	m_elements.clear();

}

template<typename index_type, typename priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::orderUp(ElementListIt i) {

	assert(i != m_elements.end() && L"Invalid iterator passed to function");

	value_type vt = (*i);

	i = m_elements.erase(i);

	while(i != m_elements.end()) {

		if(compare(vt, (*i)) > 0) {

			m_elements.insert(i, vt);

			return;
		}

		++i;
	}

	m_elements.push_back(vt);

}

template<class index_type, class priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::orderUp(const value_type& val)
{
	for(ElementListIt i = m_elements.begin(); i != m_elements.end(); ++i)
	{
		assert(val.first != i->first);

		if(compare(val, (*i)) > 0)
		{
			assert(val.first != i->first);

			m_elements.insert(i, val);

			return;
		}
	}

	m_elements.push_back(val);
}

template<typename index_type, typename priority_type>
void FIFE::PriorityQueue<index_type, priority_type>::orderDown(ElementListIt i) {

	assert(i != m_elements.end());

	value_type vt = (*i);

	i = m_elements.erase(i);

	if(i == m_elements.end()) {
		--i;
	}

	ElementListIt j = i;

	++j;

	while(i != m_elements.begin()) {
		if(compare(vt, (*i)) < 0) {

			m_elements.insert(j, vt);

			return;
		}

		--i;
		--j;
	}

	m_elements.push_front(vt);
}

template<typename index_type, typename priority_type>
int32_t FIFE::PriorityQueue<index_type, priority_type>::compare(const value_type& a, const value_type& b) {

	if(m_ordering == Descending) {

		if(a.second > b.second) {
			return 1;
		} else if(b.second > a.second) {
			return -1;
		}

	} else {

		if(a.second < b.second) {
			return 1;
		} else if(b.second < a.second) {
			return -1;
		}
	}

	return 0;
}

#endif
