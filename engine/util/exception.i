%module exception
%{
#include "util/exception.h"
%}

namespace FIFE {
	class Exception {
	public:
		Exception(const std::string& txt);
		virtual ~Exception();
		const std::string& getMessage() const;
	};
	
	class SDLException : public Exception {
	public:
		SDLException(const std::string& msg) : Exception("SDLException: " + msg) {}
	};
	
	class NotFound : public Exception {
	public:
		NotFound(const std::string& msg);
	};
	class IndexOverflow : public Exception {
	public:
		IndexOverflow(const std::string& s);
	};
	class InvalidFormat : public Exception {
	public:
		InvalidFormat(const std::string& s);
	};
	class CannotOpenFile : public Exception {
	public:
		CannotOpenFile(const std::string& s);
	};
	class OutOfMemory : public Exception {
	public:
		OutOfMemory(const std::string& s);
	};
	class InvalidConversion : public Exception {
	public:
		InvalidConversion(const std::string& s);
	};
	class NotSupported : public Exception {
	public:
		NotSupported(const std::string& s);
	};
	class NameClash : public Exception {
	public:
		NameClash(const std::string& s);
	};
	class Duplicate : public Exception {
	public:
		Duplicate(const std::string& s);
	};
	class ScriptException : public Exception {
	public:
		ScriptException(const std::string& s);
	};
	class EventException : public Exception {
	public:
		EventException(const std::string& s);
	};
	class GuiException : public Exception {
	public:
		GuiException(const std::string& s);
	};

}