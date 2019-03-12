#ifndef STUDENT_OPTIONS_SAVE_STUDENT_H
#define STUDENT_OPTIONS_SAVE_STUDENT_H
#include "..\screen.h"

namespace gui {
	namespace student_subscreens {
		class save_student : public screen {
		public:
			save_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void save();
		};
	}
}

#endif