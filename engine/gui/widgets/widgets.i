%module(directors="1") widgets
%{
#include <guichan.hpp>
%}

namespace gcn {
	%feature("director") ListModel;
	class ListModel
	{
	public:
		virtual ~ListModel() { }
		virtual int getNumberOfElements() = 0;
		virtual std::string getElementAt(int i) = 0;
	};
}

