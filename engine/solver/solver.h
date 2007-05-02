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


#ifndef FIFE_SOLVER_SOLVER_H
#define FIFE_SOLVER_SOLVER_H

// Standard C++ library includes
#include <vector>

// 3rd party library includes
#include <boost/function.hpp>

// FIFE includes
// These includes are split up in two parts, separated by one empty line
// First block: files included from the FIFE root src directory
// Second block: files included from the same folder
#include "graph.h"

namespace FIFE { namespace solver {

	/**
		A container for a solution that consists a vector of States that
		describes a path through the Graph representing the search space.
        */
	struct Solution {
		public:
			enum Status {
				SOLUTION_FOUND, 	/// A solution path has been found
				NO_SOLUTION,		/// The search has completed without finding a valid solution
				INCOMPLETE		/// The search has not yet completed
			};

			Status m_status;
			std::vector< void* > m_path;			
	};

	/**
		A pure abstract class used to define an interface for a cooperatively scheduled (one
                that can be run in small chunks of processing) solver that operates on Graphs of States.

		Expected implementations might employ algorithms such as A-Star, Hierarchical A-Star.
                In the future it might be worth investigating how to allow multiple paths to be found
                from a single point using an algorithm like Dijkstra's algorithm.

	*/
	class Solver {

	  public:
		typedef boost::function1<void, const Solution&> SearchCompleteCallback;

		virtual ~Solver() = 0;


                virtual void setSearchSpace( Graph& searchSpace ) = 0;


		/**
			Start searching for the path from start to end.

			@param startState	Input, the starting state for the path.
			@param endState		Input, the ending state for the path.
			@return				True indicating the search has completed, FALSE indicating the 
							 search has not yet completed (calling continueSearch will
							 allow the search to continue )
		*/
		virtual bool startSearch( void* startState, 
		                          void* endState, 
		                          SearchCompleteCallback resultCallback ) = 0;

		virtual bool continueSearch( ) = 0;

		virtual void stopSearch( ) = 0;

                virtual const Solution& result() = 0;

		/// Should be called whenever the cost between states or the connection between states changes.
		virtual void Reset() = 0;

		/*  
			@warning It is very easy to "miss" a state that needs to be changed and break the solver.

			As described on the main page, it is possible to change the costs (but not the connections)
			of states without calling Reset(). Be sure that all effected states are passed to
			StateCostChange.
		*/
		virtual void StateCostChange( void* state ) = 0;

        };
    
} } // FIFE::solver

#endif

