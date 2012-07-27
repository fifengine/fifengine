%module librocket
%{
#include <Rocket/Core.h>
%}

namespace Rocket {
	namespace Core {
	
		%feature("director") EventListener;
		%nodefaultctor EventListener;
		class EventListener {
		public:
		};
	
		%nodefaultctor Element;
		class Element {
		public:
			%extend {
				void addEventListener(const std::string& event, EventListener* listener, bool in_capture_phase=false) {
					Rocket::Core::String rocketEvent(event.c_str());
					$self->AddEventListener(rocketEvent, listener, in_capture_phase);
				}
				
				void removeEventListener(const std::string& event, EventListener* listener, bool in_capture_phase=false) {
					Rocket::Core::String rocketEvent(event.c_str());
					$self->RemoveEventListener(rocketEvent, listener, in_capture_phase);
				}
				
				void setInnerRML(const std::string& rml) {
					Rocket::Core::String rocketRml(rml.c_str());
					$self->SetInnerRML(rocketRml);
				}
				
				std::string getInnerRML() {
					Rocket::Core::String out;
					$self->GetInnerRML(out);
					return std::string(out.CString());
				}
			}
		};
	
		%nodefaultctor ElementDocument;
		class ElementDocument {
		public:
			%rename(show) Show();
			void Show();
			%rename(hide) Hide();
			void Hide();
			
			%extend {
				Element* getElementById(const std::string& id) {
				    Rocket::Core::String rocketId(id.c_str());
				    return $self->GetElementById(rocketId);
				}
			}
		};
	}
}