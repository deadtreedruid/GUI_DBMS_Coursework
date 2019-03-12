#ifndef COURSE_SAVE_COURSE_TO_FILE_H
#define COURSE_SAVE_COURSE_TO_FILE_H
#include "..\screen.h"

namespace gui {
	namespace course_subscreens {
		class save_course_to_file : public screen {
		public:
			save_course_to_file();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void save();
		};
	}
}

#endif