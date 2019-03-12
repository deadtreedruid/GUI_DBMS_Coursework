#ifndef COURSE_OPTIONS_ADD_ASSESSMENT_H
#define COURSE_OPTIONS_ADD_ASSESSMENT_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\course.h"

namespace gui {
	namespace course_subscreens {
		class add_assessment : public screen {
		public:
			add_assessment();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void add();
			void find_course();

			util::linked_list<course>::node_t *m_course = nullptr;
		};
	}
}

#endif