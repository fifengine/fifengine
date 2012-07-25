%module librocket
%{
#include <Rocket/Core.h>
%}

namespace Rocket {
	namespace Core {
	
		%nodefaultctor ElementDocument;
		//rename methods to conform with fife coding standards
		%rename(show) ElementDocument::Show(); 
		%rename(hide) ElementDocument::Hide();
		class ElementDocument {
		public:
			void Show();
			void Hide();
		};
	}
}