%module settingsmanager
%{
#include "util/settingsmanager.h"
%}
%include "std_string.i"

namespace FIFE {
	class SettingsManager {
	public:
		SettingsManager();
		virtual ~SettingsManager();
		void loadSettings(const std::string& settings_file_name);
		void saveSettings(const std::string& settings_file_name, bool create_on_failure=false) const;

		template <typename T> T read(const std::string& key, const T& def);
		%template(read_int) read<int>;
		%template(read_bool) read<bool>;
		%template(read_string) read<std::string>;

		template <typename T> void write(const std::string& key, const T& value);
		%template(write_int) write<int>;
		%template(write_bool) write<bool>;
		%template(write_string) write<std::string>;
	};
}