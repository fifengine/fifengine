%module fife
%{
#include "util/attributedclass.h"
#include "util/point.h"
#include "util/rect.h"
#include "util/fifeclass.h"
%}

%include "util/point.i"
%include "util/rect.h"
%include "util/fifeclass.i"

namespace FIFE {

	class AttributedClass: public FifeClass {
	public:
		AttributedClass(const std::string& identifier);
		~AttributedClass();

		const std::string& Id() const;

		virtual std::vector<std::string> listFields() const;
		virtual const std::string& get(const std::string& field);
		virtual void set(const std::string& field, const std::string& value);
		void remove(const std::string& field);
	};
}
