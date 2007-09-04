%module pather
%{
#include "pathfinder/linearpather/linearpather.h"
#include "model/metamodel/abstractpather.h"
%}


namespace FIFE {

	class AbstractPather {
	public:
		AbstractPather();
		virtual ~AbstractPather();
		virtual void setMap(Map* map) = 0;
		virtual int getNextLocations(const Location& curpos, const Location& target, 
					     std::vector<Location>& nextlocations, const int session_id) = 0;
	};
	
	class LinearPather: public AbstractPather {
	public:
		LinearPather();
		virtual ~LinearPather();
		virtual void setMap(Map* map);
		virtual int getNextLocations(const Location& curpos, const Location& target, 
					     std::vector<Location>& nextlocations, const int session_id);
	};
}
