#ifndef ENROL_OPTIONS_UNENROL_H
#define ENROL_OPTIONS_UNENROL_H
#include "..\screen.h"

namespace gui {
	namespace enrol_subscreens {
		class unenrol_student : public screen {
		public:
			unenrol_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void unenrol();
		};
	}
}

#endif