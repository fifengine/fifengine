%module util 
%{
#include "util/attributedclass.h"
%}

%include "util/point.h"
%include "util/rect.h"

namespace FIFE {

	class AttributedClass {
		public:

			AttributedClass(const std::string& class_name);
			~AttributedClass();

			template<typename T>
			const T& get(const std::string& field, const T& value = T());
			%template(get_bool) get<bool>;
			%template(get_int) get<long>;
			%template(getPoint) get<Point>;
			%template(getRect) get<Rect>;
			%template(get_string) get<std::string>;

			template<typename T>
			void set(const std::string& field, const T& value);
			%template(set_bool) set<bool>;
			%template(set_int) set<long>;
			%template(setPoint) set<Point>;
			%template(setRect) set<Rect>;
			%template(set_string) set<std::string>;

			void remove(const std::string& field);

			bool hasField(const std::string& field);

		private:
	};
}
