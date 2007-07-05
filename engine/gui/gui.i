%module(directors="1") gui
%{
#include <guichan.hpp>
%}

namespace gcn {
	class Color
	{
	public:
		Color();
		Color(int color);
		Color(int r, int g, int b, int a = 255);
		int r;
		int g;
		int b;
		int a;
	};

	%feature("director") ListModel;
	class ListModel
	{
	public:
		virtual ~ListModel() { }
		virtual int getNumberOfElements() = 0;
		virtual std::string getElementAt(int i) = 0;
	};


}

