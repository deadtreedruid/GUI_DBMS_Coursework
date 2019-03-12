#ifndef STUDENT_OPTIONS_ADD_STUDENT_H
#define STUDENT_OPTIONS_ADD_STUDENT_H
#include "..\screen.h"

namespace gui {
	namespace student_subscreens {
		class add_student : public screen {
		public:
			add_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void submit();
			void reset();
		};
	}
}

#endif