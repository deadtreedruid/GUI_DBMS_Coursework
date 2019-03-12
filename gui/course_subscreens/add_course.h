#ifndef COURSE_OPTIONS_ADD_COURSE_H
#define COURSE_OPTIONS_ADD_COURSE_H
#include "..\screen.h"

namespace gui {
	namespace course_subscreens {
		class add_course : public screen {
		public:
			add_course();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void submit();
		};
	}
}

#endif