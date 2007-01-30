/*
Copyright (c) 2000-2005 Lee Thomason (www.grinninglizard.com)

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

#ifdef _MSC_VER
#pragma warning( disable : 4786 )	// Debugger truncating names.
#pragma warning( disable : 4530 )	// Exception handler isn't used
#endif

#include <vector>
#include <map>
#include <algorithm>
#include <functional>

#include <memory.h>
#include <stdio.h>

//#define DEBUG_PATH
//#define DEBUG_PATH_DEEP

#include "micropather.h"

using namespace std;
using namespace micropather;

template<class _Ty>
	struct PathNodeCompareTotalCost : binary_function<_Ty, _Ty, bool> {
	bool operator()(const _Ty& _X, const _Ty& _Y) const
		{return (_X->totalCost > _Y->totalCost ); }
	};

namespace micropather {

class OpenQueue
{
  public:
	OpenQueue( Graph* _graph )
	{ 
		graph = _graph; 
		#ifdef USE_LIST
		sentinel = (PathNode*) sentinelMem;
		sentinel->Init( 0, 0, FLT_MAX, FLT_MAX, 0 );
		sentinel->totalCost = FLT_MAX;
		sentinel->next = sentinel;
		sentinel->prev = sentinel;
			#ifdef DEBUG
				sentinel->CheckList();
			#endif
		#endif
	}
	~OpenQueue()	{}

	void Push( PathNode* pNode )
	{

		assert( pNode->inOpen == 0 );
		assert( pNode->inClosed == 0 );

		#ifdef DEBUG_PATH_DEEP
			printf( "Open Push: " );
			graph->PrintStateInfo( pNode->state );
			printf( " total=%.1f\n", pNode->totalCost );		
		#endif

		#ifdef USE_LIST
		// Add sorted. Lowest to highest cost path. Note that the sentinel has
		// a value of FLT_MAX, so it should always be sorted in.
		assert( pNode->totalCost < FLT_MAX );
		PathNode* iter = sentinel->next;
		while ( true )
		{
			if ( pNode->totalCost < iter->totalCost ) {
				iter->AddBefore( pNode );
				pNode->inOpen = 1;
				break;
			}
			iter = iter->next;
		}
		assert( pNode->inOpen );	// make sure this was actually added.
			#ifdef DEBUG
				sentinel->CheckList();
			#endif
		#else
		heapVector.push_back( pNode );
		#endif
	}

	PathNode* Pop()
	{
		#ifdef USE_LIST
		assert( sentinel->next != sentinel );
		PathNode* pNode = sentinel->next;
		pNode->Unlink();
			#ifdef DEBUG
				sentinel->CheckList();
			#endif
		#else
		PathNode* pNode = heapVector[0];

		const int size = heapVector.size();
		int found = 0;
		for( int i=1; i<size; ++i ) {
	   		if ( heapVector[i]->totalCost < pNode->totalCost ) {
	   			pNode = heapVector[i];
	   			found = i;
			}				
		}   
		if ( found < size-1 ) 
			memcpy( &heapVector[found], &heapVector[found+1], sizeof( PathNode* ) * (size-found-1) );
		heapVector.pop_back();
		#endif

		assert( pNode->inClosed == 0 );
		assert( pNode->inOpen == 1 );
		pNode->inOpen = 0;

		#ifdef DEBUG_PATH_DEEP
			printf( "Open Pop: " );
			graph->PrintStateInfo( pNode->state );
			printf( " total=%.1f\n", pNode->totalCost );		
		#endif

		return pNode;
	}

	void Update( PathNode* pNode )
	{
		#ifdef DEBUG_PATH_DEEP
			printf( "Open Update: " );		
			graph->PrintStateInfo( pNode->state );
			printf( " total=%.1f\n", pNode->totalCost );		
		#endif
		
		#ifdef USE_LIST
		assert( pNode->inOpen );

		// If the node now cost less than the one before it,
		// move it to the front of the list.
		if ( pNode->prev != sentinel && pNode->totalCost < pNode->prev->totalCost ) {
			pNode->Unlink();
			sentinel->next->AddBefore( pNode );
		}

		// If the node is too high, move to the right.
		if ( pNode->totalCost > pNode->next->totalCost ) {
			PathNode* it = pNode->next;
			pNode->Unlink();

			while ( pNode->totalCost > it->totalCost )
				it = it->next;

			it->AddBefore( pNode );
				#ifdef DEBUG
					sentinel->CheckList();
				#endif
		}
		#endif
	}
    
	#ifdef USE_LIST
	bool Empty()	{ return sentinel->next == sentinel; }
	#else
	bool Empty()	{ return heapVector.empty(); }
	#endif

  private:
	OpenQueue( const OpenQueue& );	// undefined and unsupported
	void operator=( const OpenQueue& );
  
	#ifdef USE_LIST
	PathNode* sentinel;
	int sentinelMem[ ( sizeof( PathNode ) + sizeof( int ) ) / sizeof( int ) ];
	#else
	vector< PathNode* > heapVector;
	#endif
	Graph* graph;	// for debugging
};


class ClosedSet
{
  public:
	ClosedSet( Graph* _graph )		{ this->graph = _graph; }
	~ClosedSet()	{}

	void Add( PathNode* pNode )
	{
		#ifdef DEBUG_PATH_DEEP
			printf( "Closed add: " );		
			graph->PrintStateInfo( pNode->state );
			printf( " total=%.1f\n", pNode->totalCost );		
		#endif
		#ifdef DEBUG
		assert( pNode->inClosed == 0 );
		assert( pNode->inOpen == 0 );
		#endif
		pNode->inClosed = 1;
	}

	void Remove( PathNode* pNode )
	{
		#ifdef DEBUG_PATH_DEEP
			printf( "Closed remove: " );		
			graph->PrintStateInfo( pNode->state );
			printf( " total=%.1f\n", pNode->totalCost );		
		#endif
		assert( pNode->inClosed == 1 );
		assert( pNode->inOpen == 0 );

		pNode->inClosed = 0;
	}

  private:
	ClosedSet( const ClosedSet& );
	void operator=( const ClosedSet& );
	Graph* graph;
};

} 

MicroPather::MicroPather( unsigned allocate,
			  unsigned limit )
	: ALLOCATE( allocate ),
	  BLOCKSIZE( allocate-1 ),
	  graph( 0 ),
	  pathNodeMem( 0 ),
	  availMem( 0 ),
	  pathNodeCount( 0 ),
	  frame( 0 ),
	  checksum( 0 ),
	  iterationLimit( limit )
{
}


MicroPather::~MicroPather()
{
	if (open)
	{
		delete open;
	}

	if (closed)
	{
		delete closed;
	}


	PathNode *temp;

	while( pathNodeMem ) {
		temp = pathNodeMem;
		pathNodeMem = pathNodeMem[ ALLOCATE-1 ].left;
		free( temp );	// c-allocation
	}
}

void MicroPather::setSearchSpace( Graph& searchSpace )
{	
	graph = &searchSpace;
	memset( hashTable, 0, sizeof( PathNode* )*HASH_SIZE );
}

void MicroPather::DumpHashTable()
{
	for( int i=0; i<HASH_SIZE; ++i )
	{
    	int count = 0;
    	RecPathNodeWalk( hashTable[i], &count );
    	if ( count != 0 )
    	   printf( "Hashtable[%3d] has %d items\n", i, count );
    }    
}    

void MicroPather::RecPathNodeWalk( PathNode* node, int* count )
{
    if ( node ) 
    {
        *count += 1;
        if ( node->left )
            RecPathNodeWalk( node->left, count );
		#ifdef USE_BINARY_HASH
        if ( node->right )
            RecPathNodeWalk( node->right, count );
		#endif
    }
}

    
void MicroPather::Reset()
{
	while( pathNodeMem ) {
		PathNode* temp = pathNodeMem;
		pathNodeMem = pathNodeMem[ ALLOCATE-1 ].left;
		free( temp );	// c-allocation
	}
	pathNodeMem = 0;
	memset( hashTable, 0, sizeof( PathNode* )*HASH_SIZE );
	pathNodeMem = 0;
	availMem = 0;
	pathNodeCount = 0;
	frame = 0;
	checksum = 0;
}


void MicroPather::StateCostChange( void* state )
{
	PathNode* node = FindPathNode( state, true );
	if ( node ) {
		node->numAdjacent = -1;	// throw away any cached data.
	}
}


PathNode* MicroPather::AllocatePathNode() 
{
	if ( availMem == 0 ) {
		PathNode* newBlock = (PathNode*) malloc( sizeof(PathNode) * ALLOCATE );
		// set up the "next" pointer
		newBlock[ALLOCATE-1].left = pathNodeMem;
		pathNodeMem = newBlock;
		availMem = BLOCKSIZE;
	}
	assert( availMem );

	PathNode* result = pathNodeMem + (BLOCKSIZE	- availMem );
	--availMem;
	++pathNodeCount;
	return result;
}


PathNode* MicroPather::NewPathNode( void* state, float costFromStart, float estToEnd, PathNode* parent )
{
	// Try to find an existing node for this state.
	unsigned key = Hash( state );   //(HASH_SIZE-1) & ( (unsigned)state + (((unsigned)state)>>8) + (((unsigned)state)>>16) + (((unsigned)state)>>24) );

	if ( !hashTable[key] ) {
		// There isn't even a hashtable yet - create and initialize the PathNode.
		hashTable[key] = AllocatePathNode();
		hashTable[key]->Init( frame, state, costFromStart, estToEnd, parent );
		return hashTable[key];
	}

	PathNode* root = hashTable[key];
	PathNode* up = 0;
	while ( root ) {
		up = root;
		if ( root->state == state ) {
			root->Reuse( frame, costFromStart, estToEnd, parent );
			assert( root->state == state );
			return root;
		}
		#ifdef USE_BINARY_HASH
		else if ( state > root->state ) {
			root = root->right;
		}
		#endif
		else {
			#ifdef USE_BINARY_HASH
			assert( state < root->state );
			#endif
			root = root->left;
		}
	}

	assert( up );
	PathNode* pNode = AllocatePathNode();
	pNode->Init( frame, state, costFromStart, estToEnd, parent );
	#ifdef USE_BINARY_HASH
	if ( state > up->state ) {
		assert( up->right == 0 );
		up->right = pNode;
	}
	else {
		assert( up->left == 0 );
		up->left = pNode;
	}
	#else
	up->left = pNode;
	#endif
	return pNode;
}


void MicroPather::GoalReached( PathNode* node, void* start, void* end, vector< void* > *path )
{
	path->clear();

	// We have reached the goal.
	// How long is the path? Used to allocate the vector which is returned.
	int count = 1;
	PathNode* it = node;
	while( it->parent )
	{
		++count;
		it = it->parent;
	}			

	// Now that the path has a known length, allocate
	// and fill the vector that will be returned.
	if ( count < 3 )
	{
		// Handle the short, special case.
		path->resize(2);
		(*path)[0] = start;
		(*path)[1] = end;
	}
	else
	{
		path->resize(count);

		(*path)[0] = start;
		(*path)[count-1] = end;

		count-=2;
		it = node->parent;

		while ( it->parent )
		{
			(*path)[count] = it->state;
			it = it->parent;
			--count;
		}
	}
	checksum = 0;
	#ifdef DEBUG_PATH
	printf( "Path: " );
	int counter=0;
	#endif
	for ( unsigned k=0; k<path->size(); ++k )
	{
		checksum += ((UPTR)((*path)[k])) << (k%8);

		#ifdef DEBUG_PATH
		graph->PrintStateInfo( (*path)[k] );
		printf( " " );
		++counter;
		if ( counter == 8 )
		{
			printf( "\n" );
			counter = 0;
		}
		#endif
	}
	#ifdef DEBUG_PATH
	printf( "Cost=%.1f Checksum %d\n", node->costFromStart, checksum );
	#endif
}


NodeCost* MicroPather::GetNodeNeighbors( PathNode* node, std::vector< StateCost >* neighbor, std::vector< NodeCost >* neighborNode )
{
	NodeCost* nodeCost = 0;

	if (    node->numAdjacent < 0 
		 || node->numAdjacent > PathNode::MAX_CACHE )
	{
		// It has not been computed yet (<0) or
		// can not be cached (>MAX)
		neighbor->resize( 0 );
		graph->AdjacentCost( node->state, neighbor );
		#ifdef DEBUG
		{
			// If this assert fires, you have passed a state
			// as its own neighbor state. This is impossible --
			// bad things will happen.
			for ( unsigned i=0; i<neighbor->size(); ++i )
				assert( (*neighbor)[i].state != node->state );
		}
		#endif
		node->numAdjacent = (int) neighbor->size();

		// Now convert to pathNodes, and put in cache if possible.
		if ( node->numAdjacent <= PathNode::MAX_CACHE ) 
		{
			// Can fit in the cache:
			for( int i=0; i<node->numAdjacent; ++i ) 
			{
				node->adjacent[i].cost = (*neighbor)[i].cost;
				node->adjacent[i].node = FindPathNode( (*neighbor)[i].state );
				if ( !node->adjacent[i].node ) 
				{
					node->adjacent[i].node = NewPathNode( (*neighbor)[i].state, FLT_BIG, FLT_BIG, 0 );
				}
			}
			nodeCost = node->adjacent;

			#ifdef DEBUG_PATH_DEEP
				printf( "State " );
				graph->PrintStateInfo( node->state );
				printf( "--> cache\n" );
			#endif
		}
		else {
			// Too big for cache.
			node->numAdjacent = (int) neighbor->size();

			neighborNode->resize( neighbor->size() );

			for( unsigned i=0; i<neighborNode->size(); ++i ) {
				(*neighborNode)[i].cost = (*neighbor)[i].cost;
				(*neighborNode)[i].node = FindPathNode( (*neighbor)[i].state );
				if ( !(*neighborNode)[i].node ) 
				{
					(*neighborNode)[i].node = NewPathNode( (*neighbor)[i].state, FLT_BIG, FLT_BIG, 0 );
				}
			}
			nodeCost = &(*neighborNode)[0];
			#ifdef DEBUG_PATH_DEEP
				printf( "State " );
				graph->PrintStateInfo( node->state );
				printf( "no cache\n" );
			#endif
		}
	}
	else {
		// In the cache!
		nodeCost = node->adjacent;

		for( int i=0; i<node->numAdjacent; ++i ) {
			if ( node->adjacent[i].node->frame != frame )
				node->adjacent[i].node->Reuse( frame, FLT_BIG, FLT_BIG, 0 );
		}
		#ifdef DEBUG_PATH_DEEP
			printf( "State " );
			graph->PrintStateInfo( node->state );
			printf( "cache HIT\n" );
		#endif
	}
	assert( nodeCost );

	return nodeCost;
}


#ifdef DEBUG
void MicroPather::DumpStats()
{
	int hashTableEntries = 0;
	for( int i=0; i<HASH_SIZE; ++i )
		if ( hashTable[i] )
			++hashTableEntries;
	
	int pathNodeBlocks = 0;
	for( PathNode* node = pathNodeMem; node; node = node[ALLOCATE-1].left )
		++pathNodeBlocks;

	printf( "HashTableEntries=%d/%d PathNodeBlocks=%d [%dk] PathNodes=%d SolverCalled=%d\n",
			  hashTableEntries, HASH_SIZE, pathNodeBlocks, 
			  pathNodeBlocks*ALLOCATE*sizeof(PathNode)/1024,
			  pathNodeCount,
			  frame );
}
#endif


void MicroPather::StatesInPool( std::vector< void* >* stateVec )
{
 	stateVec->clear();
	
    for ( PathNode* mem = pathNodeMem; mem; mem = mem[ALLOCATE-1].left )
    {
        unsigned count = BLOCKSIZE;
        if ( mem == pathNodeMem )
        	count = BLOCKSIZE - availMem;
    	
    	for( unsigned i=0; i<count; ++i )
    	{
    	    if ( mem[i].frame == frame )
	    	    stateVec->push_back( mem[i].state );
    	}    
	}           
}    

bool MicroPather::startSearch( void* startState, 
		               void* endState, 
		               SearchCompleteCallback callback )
{
	startNode = startState;
	endNode = endState;
	resultCallback = callback;

	solution.m_status = Solution::INCOMPLETE;	
	solution.m_path.clear();

	#ifdef DEBUG_PATH
	printf( "Path: " );
	graph->PrintStateInfo( startNode );
	printf( " --> " );
	graph->PrintStateInfo( endNode );
	printf( " min cost=%f\n", graph->LeastCostEstimate( startNode, endNode ) );
	#endif

	if ( startNode == endNode )
	{
		solution.m_status = Solution::SOLUTION_FOUND;	
		return true;
	}

	++frame;
//	// In "reuse" mode, reset the costs.
//	if ( pathNodeMem )
//		ReuseAll();

        // ToDo: better memory management - at the moment this is a hack
	if (open)
	{
		delete open;
	}

	if (closed)
	{
		delete closed;
	}

	open = new OpenQueue( graph );
	closed = new ClosedSet( graph );
	
	open->Push( NewPathNode( startNode,		// node
				0,			// cost from start
				graph->LeastCostEstimate( startNode, endNode ),
				0 ) );

	neighborVec.resize(0);
	neighborNodeVec.resize(0);

	return continueSearch();
}

bool MicroPather::continueSearch()
{
	unsigned limit = iterationLimit;

	while ( !open->Empty() )
	{
		// If the search has exceeded its limit return and wait to be contined
		if ( limit == 0 )
		{
			return false;
		}
		--limit;

		PathNode* node = open->Pop();
		
		if ( node->state == endNode )
		{
			GoalReached( node, startNode, endNode, &solution.m_path );

			#ifdef DEBUG_PATH
			DumpStats();
			#endif

			solution.m_status = Solution::SOLUTION_FOUND;	
			resultCallback( solution );
			return true;
		}
		else
		{
			// We have not reached the goal - add the neighbors.
			NodeCost* nodeCost = GetNodeNeighbors( node, &neighborVec, &neighborNodeVec );

			for( int i=0; i<node->numAdjacent; ++i )
			{
				if ( nodeCost[i].cost == FLT_MAX ) {
					continue;
				}

				float newCost = node->costFromStart + nodeCost[i].cost;

				PathNode* inOpen   = nodeCost[i].node->inOpen ? nodeCost[i].node : 0;
				PathNode* inClosed = nodeCost[i].node->inClosed ? nodeCost[i].node : 0;
				assert( !( inOpen && inClosed ) );
				PathNode* inEither = inOpen ? inOpen : inClosed;

				assert( inEither != node );

				if ( inEither )
				{
    				// Is this node is in use, and the cost is not an improvement,
    				// continue on.
					if ( inEither->costFromStart <= newCost )
						continue;	// Do nothing. This path is not better than existing.

					// Groovy. We have new information or improved information.
					inEither->parent = node;
					inEither->costFromStart = newCost;
					inEither->estToGoal = graph->LeastCostEstimate( inEither->state, endNode );
					inEither->totalCost = inEither->costFromStart + inEither->estToGoal;
				}

				if ( inClosed )
				{
					closed->Remove( inClosed );
					open->Push( inClosed );
				}	
				else if ( inOpen )
				{
					// Need to update the sort!
					open->Update( inOpen );
				}
				else if (!inEither)
				{
					assert( !inEither );
					assert( nodeCost[i].node );

					PathNode* pNode = nodeCost[i].node;
					pNode->parent = node;
					pNode->costFromStart = newCost;
					pNode->estToGoal = graph->LeastCostEstimate( pNode->state, endNode ),
					pNode->totalCost = pNode->costFromStart + pNode->estToGoal;
					
					open->Push( pNode );
				}
			}
		}					
		closed->Add( node );
	}
	#ifdef DEBUG_PATH
	DumpStats();
	#endif

	solution.m_status = Solution::NO_SOLUTION;	
	return true;
}	


