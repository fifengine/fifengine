/*
Copyright (c) 2000-2003 Lee Thomason (www.grinninglizard.com)
Grinning Lizard Utilities.

This software is provided 'as-is', without any express or implied 
warranty. In no event will the authors be held liable for any 
damages arising from the use of this software.

Permission is granted to anyone to use this software for any 
purpose, including commercial applications, and to alter it and 
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must 
not claim that you wrote the original software. If you use this 
software in a product, an acknowledgment in the product documentation 
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and 
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source 
distribution.
*/

/***************************************************************************
 *
 * This version has been modified by the FIFE team, original based on the
 *  micropather 1.1.1 release.
 *
 **************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007 by the FIFE Team                                   *
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

#ifndef FIFE_SOLVER_GRAPH_H
#define FIFE_SOLVER_GRAPH_H

// Standard C++ library includes
#include <float.h>
#include <vector>

// 3rd party library includes

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder

namespace FIFE { namespace solver {

	/**
		Used to pass the cost of states from the client application to a slover. This
		structure is copied in a vector.

		@sa AdjacentCost
	*/
	struct StateCost {
		void* state;			///< The state as a void*
		float cost;			///< The cost to the state. Use FLT_MAX for infinite cost.
	};

	/**
		A pure abstract class used to define a set of callbacks. 
		The client application inherits from 
		this class, and the methods will be called when a solver is invoked.

		The notion of a "state" is very important. It must have the following properties:
		- Unique
		- Unchanging (during a solving run is called)

		If the client application represents states as objects, then the state is usually
		just the object cast to a void*. If the client application sees states as numerical
		values, (x,y) for example, then state is an encoding of these values. The solver
		never interprets or modifies the value of state.
	*/
	class Graph {
	  public:
		virtual ~Graph() {}
	  
		/**
			Return the least possible cost between 2 states. For example, if your pathfinding 
			is based on distance, this is simply the straight distance between 2 points on the 
			map. If you pathfinding is based on minimum time, it is the minimal travel time 
			between 2 points given the best possible terrain.
		*/
		virtual float LeastCostEstimate( void* stateStart, void* stateEnd ) = 0;

		/** 
			Return the exact cost from the given state to all its neighboring states. This
			may be called multiple times, or might be cached by a solver. It *must* return the same
			exact values for every call to while a solver is processing this graph. It should 
                        generally be a simple, fast function with no callbacks into the solver.
		*/
		virtual void AdjacentCost( void* state, std::vector< StateCost > *adjacent ) = 0;

		/**
			This function is only used in DEBUG mode - it dumps output to stdout. Since void* 
			aren't really human readable, normally you print out some concise info (like "(1,2)") 
			without an ending newline.
		*/
		virtual void  PrintStateInfo( void* state ) = 0;
	};

} } // FIFE::solver

#endif
