#ifndef ENROL_OPTIONS_ENROL_FROM_FILE_H
#define ENROL_OPTIONS_ENROL_FROM_FILE_H
#include "..\screen.h"

namespace gui {
	namespace enrol_subscreens {
		class enrol_from_file : public screen {
		public:
			enrol_from_file();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void enrol();
		};
	}
}

#endif