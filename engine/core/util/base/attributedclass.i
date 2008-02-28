%module fife
%{
#include "util/base/attributedclass.h"
#include "util/structures/point.h"
#include "util/structures/rect.h"
#include "util/base/fifeclass.h"
%}

%include "util/structures/point.i"
%include "util/structures/rect.h"
%include "util/base/fifeclass.i"

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
