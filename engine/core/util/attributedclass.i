%module util 
%{
#include "util/attributedclass.h"
%}

namespace FIFE {

	class AttributedClass {
		public:

			AttributedClass(const std::string& class_name);
			~AttributedClass();

			template<typename T>
			const T& get(const std::string& field, const T& value = T()) const; 

			template<typename T>
			void set(const std::string& field, const T& value);

			void remove(const std::string& field);

			bool hasField(const std::string& field);

		private:
	};
}
