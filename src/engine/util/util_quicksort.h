//http://fife.pastebin.com/702785
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
#ifndef _UTIL_QUICKSORT_H_
#define _UTIL_QUICKSORT_H_

#include "util_insertionsort.h"

/** \brief Finds the median of three numbers in an array.
 *  Finds the median between the first, middle and last element in the 
 *  array.  This assumes that operator> and operator< has been overridden 
 *  or natively works for Type.
 *  @param p_array The array to be searched.
 *  @param p_first The index of the first item in the array.
 *  @param p_size The length of the array.
 *  @param p_compare The function two compare two Types.
 */
template <class Type>
unsigned long MedianOfThree(Type& p_array, unsigned long p_first, unsigned long p_size){
	unsigned long last = p_size + p_first - 1;
	unsigned long mid = p_first + (p_size / 2);
	
	if( p_array[p_first] < p_array[mid] &&
		p_array[p_first] < p_array[last] ){
		if( p_array[mid] < p_array[last] ){
			return mid;
		}
		return last;
	}
	if( p_array[mid] < p_array[p_first] &&
		p_array[mid] < p_array[last] ){
		if( p_array[p_first] < p_array[last] ){
			return p_first;
		}
		return last;
	}
	
	if( p_array[p_first] < p_array[mid]){
		return p_first;
	}
	return mid;
}
/** \brief Performs a quick sort on the specified array.
 *  This is a very fast sorting algorithm.  It uses a pivot and subdivides
 *  the array to do the sorting.  This assumes that operator> and operator< 
 *  has been overridden or natively works for Type.
 *  @param p_array The array to be sorted.
 *  @param p_first The index of the first item in the array.
 *  @param p_size The length of the array.
 *  @param p_compare The function two compare two Types.
 */
template <class Type>
void QuickSort(Type& p_array, unsigned long p_first, unsigned long p_size){
	typeof(p_array[p_first]) pivot;
	unsigned long last = p_first + p_size - 1;
	unsigned long mid;
	unsigned long lower = p_first;
	unsigned long higher = last;
	
	if( p_size <= 5 ){
		InsertionSort(p_array, p_first, p_size);
		return;
	}
	
	mid = MedianOfThree(p_array, p_first, p_size);
	pivot = p_array[mid];
	p_array[mid] = p_array[p_first];
	
	while( lower < higher ){
		while( pivot < p_array[higher] &&
			   lower < higher )
			higher--;
		if( higher != lower ){
			p_array[lower] = p_array[higher];
			lower++;
		}
		while( pivot > p_array[lower] &&
			   lower < higher )
			lower++;
		if( higher != lower ){
			p_array[higher] = p_array[lower];
			higher--;
		}
	}
	
	p_array[lower] = pivot;
	QuickSort(p_array, p_first, lower - p_first);
	QuickSort(p_array, lower + 1, last - lower);
}
#endif
