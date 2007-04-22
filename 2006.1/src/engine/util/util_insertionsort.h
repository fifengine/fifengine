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
#ifndef _INSERTIONSORT_H_
#define _INSERTIONSORT_H_


/** \brief Sorts an array using the insertion sort.
 *  \warning This is a slow sort for anything besides small datasets.  
 *  It is useful for datasets containing 10 or less items.  This assumes that 
 *  operator> and operator< has been overridden or natively works for Type.
 *  @param p_array The array to be sorted.
 *  @param p_first The index of the first item in the array.
 *  @param p_size The length of the array.
 */
template <class Type>
void InsertionSort(Type& p_array, unsigned long p_first, unsigned long p_size){
	typeof(p_array[p_first]) key;
	unsigned long top;
	unsigned long current;
	for( top = p_first + 1; top < p_size; top++ ){
		key = p_array[top];
		current = top - 1;
		while( p_array[current] > key && current >= 0 ){
			p_array[current + 1] = p_array[current];
			current--;
		}
		p_array[current + 1] = key;
	}
}

#endif
