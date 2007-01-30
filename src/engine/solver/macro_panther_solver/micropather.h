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
 *  macropather 1.1.1 release.
 *
 * Main changes: Moving Graph into its own file
 *               Tweaking the pather to use a FIFE solver interface
 *               (the basic algorithm has not been changed)
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


#ifndef GRINNINGLIZARD_MICROPATHER_INCLUDED
#define GRINNINGLIZARD_MICROPATHER_INCLUDED

/** @mainpage MicroPather
	
	MicroPather is a path finder and A* solver (astar or a-star) written in platform independent 
	C++ that can be easily integrated into existing code. MicroPather focuses on being a path 
	finding engine for video games but is a generic A* solver. MicroPather is open source, with 
	a license suitable for open source or commercial use.

	An overview of using MicroPather is in the <A HREF="../readme.htm">readme</A> or
	on the Grinning Lizard website: http://grinninglizard.com/micropather/
*/

#include <vector>
#include <list>
#include <assert.h>
#include <float.h>

#include "solver/graph.h"
#include "solver/solver.h"

#ifdef _DEBUG
	#ifndef DEBUG
		#define DEBUG
	#endif
#endif

#ifndef GRINLIZ_TYPES_INCLUDED
	#if defined(_MSC_VER) && (_MSC_VER >= 1400 )
		#include <stdlib.h>
		typedef uintptr_t		UPTR;
	#elif defined (__GNUC__) && (__GNUC__ >= 3 )
		#include <stdlib.h>
		typedef uintptr_t		UPTR;
	#else
		// Assume not 64 bit pointers. Get a new compiler.
		typedef unsigned UPTR;
	#endif
#endif

/*  USE_LIST and USE_BINARY_HASH change the some of details the pather algorithms. They
	are set optimally in my experiments, but different applications may benefit
	from a different combination of these #defines.
*/
#define USE_LIST
//#define USE_BINARY_HASH

namespace micropather
{
	const float FLT_BIG = FLT_MAX / 2.0f;	

	// Original pather structure moved into FIFE interface
	using FIFE::solver::StateCost;


	// Original pather class moved into FIFE interface
	using FIFE::solver::Graph;

	using FIFE::solver::Solution;

	class PathNode;
	class OpenQueue;
	class ClosedSet;

	struct NodeCost
	{
		PathNode* node;
		float cost;
	};


	class PathNode
	{
		friend class none;	// Trashy trick to get rid of compiler warning, because
    						// this class has a private constructor and destructor -
    						// it can never be "new" or created on the stack, only
    						// by special allocators.
	  public:
		void Init(	unsigned _frame,
					void* _state,
					float _costFromStart, 
					float _estToGoal, 
					PathNode* _parent )
		{
			state = _state;
			costFromStart = _costFromStart;
			estToGoal = _estToGoal;
			totalCost = _costFromStart + _estToGoal;
			parent = _parent;
			frame = _frame;
			numAdjacent = -1;
			left = 0;
			#ifdef USE_BINARY_HASH
			right = 0;
			#endif
			#ifdef USE_LIST
			next = 0;
			prev = 0;
			#endif
			inOpen = 0;
			inClosed = 0;
		}

		void Reuse( unsigned _frame, float _costFromStart, float _estToGoal, PathNode* _parent ) 
		{
			costFromStart = _costFromStart;
			estToGoal = _estToGoal;
			parent = _parent;
			frame = _frame;

			inOpen = 0;
			inClosed = 0;
		}

		void *state;			// the client state
		float costFromStart;	// exact
		float estToGoal;		// estimated
		float totalCost;		// could be a function, but save some math.
		PathNode* parent;		// the parent is used to reconstruct the path
		unsigned frame;			// unique id for this path, so the solver can distinguish
								// correct from stale values

		enum {
			/*
				@enum MAX_CACHE. If you want to optimize "down deep" this is the way
				to go. MAX_CACHE determines the number of adjacent nodes cached by 
				MicroPather. If your nodes generally have 8 or 3 neighbors (common cases)
				changing this may increase performance, sometimes dramatically.
			*/
			MAX_CACHE = 4
		};
		// If there are 4 or less adjacent states, they will be cached as *nodes*.
		NodeCost adjacent[ MAX_CACHE ];
		int numAdjacent;					// -1  is unknown & needs to be queried
											// 0-4 adjacent is known & in 'adjacent'
											// 5+  known, but has to be queried from Graph

		PathNode* left;			// Binary tree, where the 'state' is what is being compared.
								// Also used as a "next" pointer for memory layout.
		#ifdef USE_BINARY_HASH
		PathNode* right;
		#endif
		#ifdef USE_LIST
		PathNode *next, *prev;
		#endif

		unsigned inOpen:1;
		unsigned inClosed:1;

		#ifdef USE_LIST
		void Unlink() {
			next->prev = prev;
			prev->next = next;
			next = prev = 0;
		}
		void AddBefore( PathNode* addThis ) {
			addThis->next = this;
			addThis->prev = prev;
			prev->next = addThis;
			prev = addThis;
		}
		#ifdef DEBUG
		void CheckList()
		{
			assert( totalCost == FLT_MAX );
			for( PathNode* it = next; it != this; it=it->next ) {
				assert( it->prev == this || it->totalCost >= it->prev->totalCost );
				assert( it->totalCost <= it->next->totalCost );
			}
		}
		#endif
		#endif

	  private:
		PathNode();
		~PathNode();
		PathNode( const PathNode& );
		void operator=( const PathNode& );
	};


	/**
		Create a MicroPather object to solve for a best path. Detailed usage notes are
		on the main page.
	*/
	class MicroPather : public FIFE::solver::Solver
	{
		friend class micropather::PathNode;

	  public:


		/**
			Construct the pather, configuring its node allocator

			@param allocate		The block size that the node cache is allocated from. In some
								cases setting this parameter will improve the perfomance of
								the pather.
								- If you have a small map, for example the size of a chessboard, set allocate
								  to be the number of states+1. 65 for a chessboard. This will allow
								  MicroPather to used a fixed amount of memory.
								- If your map is large, something like 1/4 the number of possible
								  states is good. For example, Lilith3D normally has about 16,000 
								  states, so 'allocate' should be about 4000.

			@param iterationLimit	The maximum solver iterations before pather returns pausing the current search
								if the pather has not found (but is still searching) solution
								it returns false. The pather can be continued using the continueSearch
								method.

		*/
		MicroPather( unsigned allocate = 250,
                             unsigned iterationLimit = 10000 );
		~MicroPather();

		virtual void setSearchSpace( Graph& searchSpace );

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
		                          SearchCompleteCallback resultCallback );

		virtual bool continueSearch( );

		virtual void stopSearch( ){}; /*No Functionality required for this implementation*/

                virtual const Solution& result(){ return solution; }

		/// Should be called whenever the cost between states or the connection between states changes.
		virtual void Reset();

		/*  
			@warning It is very easy to "miss" a state that needs to be changed and break the solver.

			As described on the main page, it is possible to change the costs (but not the connections)
			of states without calling Reset(). Be sure that all effected states are passed to
			StateCostChange.
		*/
		virtual void StateCostChange( void* state );



		/**
			Return the "checksum" of the last path returned by Solve(). Useful for debugging,
			and a quick way to see if 2 paths are the same.
		*/
		UPTR Checksum()	{ return checksum; }
		
		void DumpHashTable();
		
		// Debugging function to return all states that were used
		// by the last "solve" 
		void StatesInPool( std::vector< void* >* stateVec );

	  private:
		MicroPather( const MicroPather& );	// undefined and unsupported
		void operator=( const MicroPather ); // undefined and unsupported
		
		void GoalReached( PathNode* node, void* start, void* end, std::vector< void* > *path );
		void RecPathNodeWalk( PathNode* node, int* count );
		
		// Allocates and returns memory for a new, unititialized node.
		PathNode* AllocatePathNode();

		// Returns a path node that is ready to go. 
		// Strictly speaking, the name is somewhat misleading, as it
		// may be reused from the cache, but it will be ready for use regardless.
		PathNode* NewPathNode( void* state, float costFromStart, float estToEnd, PathNode* parent );

		// Finds the node for state, or returns null. 
		// A node that doesn't have a correct "frame" will never be found.
		PathNode* FindPathNode( void* state, bool findStale = false ); 

		NodeCost* GetNodeNeighbors( PathNode* node, std::vector< StateCost >* neighbor, std::vector< NodeCost >* neighborNode );

		#ifdef DEBUG
		void DumpStats();
		#endif

		enum {
			#ifdef DEBUG_PATH
			HASH_SIZE = 16,         // In debug mode, stress the allocators more than in release.
									// The HASH_SIZE must be a power of 2.
			#else
			HASH_SIZE = 256,
			#endif
		};

		unsigned Hash( void* voidval ) {
			UPTR val = (UPTR)voidval;
			return (unsigned)((HASH_SIZE-1) & ( val + (val>>8) + (val>>16) + (val>>24) ));
		}    
    
		const unsigned ALLOCATE;			// how big a block of pathnodes to allocate at once
		const unsigned BLOCKSIZE;			// how many useable pathnodes are in a block

		Graph* graph;
		PathNode* hashTable[HASH_SIZE];		// a fixed hashtable used to "jumpstart" a binary search
		PathNode* pathNodeMem;				// pointer to root of PathNode blocks
		unsigned availMem;					// # PathNodes available in the current block
		unsigned pathNodeCount;				// the # of PathNodes in use
		unsigned frame;						// incremented with every solve, used to determine
											// if cached data needs to be refreshed
		UPTR checksum;					// the checksum of the last successful "Solve".

		std::vector< StateCost > neighborVec;	// really local vars to Solve, but put here to reduce memory allocation
		std::vector< NodeCost > neighborNodeVec; // really local vars to Solve, but put here to reduce memory allocation

		unsigned iterationLimit;
		void* startNode;
		void* endNode;
		SearchCompleteCallback resultCallback;
		Solution solution;
		OpenQueue* open;
		ClosedSet* closed;
	};

	inline PathNode* MicroPather::FindPathNode( void* state, bool findStale )
	{
		unsigned key = Hash( state );

		PathNode* root = hashTable[key];
		#ifdef USE_BINARY_HASH
		while ( root ) {
			if ( root->state == state ) {
				if ( findStale || root->frame == frame )
					return root;
				else
					return 0;
			}
			else if ( state > root->state ) {
				root = root->right;
			}
			else {
				assert( state < root->state );
				root = root->left;
			}
		}
		#else
		while( root ) {
			if ( root->state == state ) {
				if ( findStale || root->frame == frame )
					return root;
				else
					return 0;
			}
			else {
				root = root->left;
			}
		}
		#endif
		return 0;
	}		
};	// namespace grinliz

#endif

