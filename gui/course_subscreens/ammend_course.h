#ifndef COURSE_OPTIONS_AMMEND_COURSE_H
#define COURSE_OPTIONS_AMMEND_COURSE_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\course.h"

namespace gui {
	namespace course_subscreens {
		class ammend_course : public screen {
		public:
			ammend_course();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void find_ammend_target();
			void ammend_target();
			util::linked_list<course>::node_t *m_ammend_target = nullptr;
		};
	}
}

#endif