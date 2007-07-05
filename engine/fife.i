%module fife
%{
#include <boost/shared_ptr.hpp>
%}

namespace boost {
	template<class T> class shared_ptr {
	public:
		T * operator-> () const;
	};
}

%include "util/exception.i"
%include "util/settingsmanager.i"
%include "util/log.i"
%include "util/time/timemanager.i"
%include "util/time/timeevent.i"
%include "eventchannel/eventchannel.i"
%include "vfs/vfs.i"
%include "audio/audiomanager.i"
%include "engine.i"
