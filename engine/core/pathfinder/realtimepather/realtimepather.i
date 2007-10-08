%module pather
%{
#include "pathfinder/realtimepather/realtimepather.h"
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
	
	class RealTimePather : public AbstractPather {
	public:
		RealTimePather();
		virtual ~LinearPather();
		virtual void setMap(Map* map);
		virtual int getNextLocations(const Location& curpos, const Location& target, 
					     std::vector<Location>& nextlocations, const int session_id);
	};
}
