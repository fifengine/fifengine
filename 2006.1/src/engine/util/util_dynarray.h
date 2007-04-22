/***************************************************************************
 *   Copyright (C) 2005-2006 by the FIFE Team                              *
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
#ifndef _UTIL_DYNARRAY_H_
#define _UTIL_DYNARRAY_H_

#include "util_quicksort.h"

/** /brief A dynamic array class similar to stl::vector.
 *  This dynamic array class provides similar functionality as stl::vector, but 
 *  provides better management of memory.  stl::vector doubles the size of 
 *  the array any time that it needs to make room for more items.  This class 
 *  provides a settable value that controls the increment for when the array 
 *  needs to be resized.  The default value is 32, but can be set to any 
 *  value that would make sense for the data being stored.  It provides 
 *  a quick sort method, however, the data being stored must have overridden 
 *  < and > for the sort method to work.
 */
template <class Type>
class DynArray
{
private:
	/// The internal array.
	Type* m_array;
	/// The total size of the stored array.
	unsigned long m_size;
	/// The length of the contents of the stored array.
	unsigned long m_length;
	/// The amount to resize the array by when more space is needed.
	unsigned long m_resize_block;
	
public:
	/** \brief Constructor.
	 *  Takes the size of an array to initially create, default 0.
	 *  @param p_size The size to make the starting array, default 0.
	 */
	DynArray(unsigned long p_size = 0){
		m_array = new Type[p_size];
		m_size = p_size;
		m_length = 0;
		m_resize_block = 32;	
	}
	/** \brief Destructor.
	 *  Clears allocated memory for the array.
	 */
	~DynArray(){
		if( m_array != 0 ){
			delete[] m_array;
		}	
		m_array = 0;
	}
	/** \brief Accessor for the length of the array contents. */
	unsigned long Length(){ return m_length; }
	/** \brief Accessor for the current length of the total array. */
	unsigned long Size(){ return m_size; }
	/** \brief Accessor for the current resize block. */
	unsigned long GetResizeBlock(){ return m_resize_block; }
	/** \brief Accessor to set the resize block. */
	void SetResizeBlock(unsigned long p_new_size){ m_resize_block = p_new_size; }
	/** \brief Resize the array to the specified size.
	 *  Resizes the array by creating a new array of the specified size, copying
	 *  the contents to it, then copying it back to the original array.  If no size 
	 *  is specified, it is incremented by the resize block, stored in m_resize_block.
	 *  @param p_size The size to make the array.
	 */
	void Resize(){
		Resize(m_size + m_resize_block);
	}
	void Resize(unsigned long p_size){
		Type* temp_array = new Type[p_size];
		unsigned long min;
		unsigned long index;
		
		if( temp_array == 0 ){
			return;	
		}
		
		if( p_size < m_size ){
			min = p_size;	
		}
		else{
			min = m_size;
		}
		
		for( index = 0; index < min; index++ ){
			temp_array[index] = m_array[index];
		}
		
		m_size = p_size;
		
		if( m_array != 0 ){
			delete[] m_array;
		}
		
		m_array = temp_array;
	}
	/** \brief Pushes an item onto the end of the array like a stack.
	 *  @param p_item The item to push onto the array.
	 */
	void Push(Type p_item){
		if( m_length >= m_size ){
			Resize();
		}
		m_array[m_length++] = p_item;
}
	/** \brief Pops an item from the array like a stack.
	 *  @return Type Returns the popped item or void if the array is empty.
	 */
	Type Pop(){
		if( m_length == 0 ){
			return 0;
		}
		return m_array[m_length--];		
	}
	/** \brief Inserts an item into a specified location in the array.
	 *  \warning  This is very slow as the contents of the array has to be
	 *  repositioned.  Use only if you know what you're doing or dealing 
	 *  with very small amounts of data.
	 *  @param p_item The item to be inserted into the array.
	 *  @param p_index The position to insert the item.
	 */
	void Insert(Type p_item, unsigned long p_index){
		if( m_length >= m_size ){
			Resize();
		}
		unsigned long index;
		
		for( index = m_length - 1; index > p_index; index-- ){
			m_array[index] = m_array[index - 1];
		}
		
		m_array[p_index] = p_item;
		m_length++;
	}
	/** \brief Remove an item from the specified location in the array.
	 *  \warning  This is a slow process as the contents of the array has to 
	 *  be repositioned.  Use only if you know what you're doing or dealing 
	 *  with very small amounts of data.
	 *  @param p_index The position of the item to remove.
	 */
	void Remove(unsigned long p_index){
		if( m_length == 0 ){
			return;
		}
		unsigned long index;
		
		for( index = p_index + 1; index < m_length; index++ ){
			m_array[index - 1] = m_array[index];
		}
		
		m_length--;
	}
	/** \brief Perform a quick sort on the contained array.
	 *  \warning operator< and operator> must have been overridden or be 
	 *  natively supported for Type in order for this to not blow up!
	 */
	void Quicksort(){
		QuickSort(m_array, 0, m_length);
	}
	/** \brief Overload so [] can be used to access members of the array. */
	Type& operator[]( unsigned long p_index ){
		return m_array[p_index];
	}
	/** \brief Overload so that this type of array can be used where methods take an 
	 *  array as a parameter.
	 */
	operator Type*(){
		return m_array;
	}
};
#endif
