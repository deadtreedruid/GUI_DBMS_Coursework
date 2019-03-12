#ifndef COURSE_OPTIONS_AMMEND_ASSESSMENT_H
#define COURSE_OPTIONS_AMMEND_ASSESSMENT_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\assessment.h"
#include "..\..\course.h"

namespace gui {
	namespace course_subscreens {
		class ammend_assessment : public screen {
		public:
			ammend_assessment();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void find_course();
			void ammend();
			util::linked_list<course>::node_t *m_course = nullptr;
		};
	}
}

#endif