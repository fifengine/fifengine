%module pather
%{
#include "pathfinder/linearpather/linearpather.h"
%}

%include "model/metamodel/abstractpather.h"

namespace FIFE { namespace model {

	class LinearPather: public AbstractPather {
	public:
		LinearPather();
		virtual ~LinearPather();
		virtual void setMap(Map* map);
		virtual int getNextCell(const Location& curpos, const Location& target, 
					Location& nextnode, const int session_id=-1);
	};

}}
