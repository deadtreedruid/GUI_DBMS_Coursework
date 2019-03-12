#ifndef COURSE_OPTIONS_SHOW_COURSE_H
#define COURSE_OPTIONS_SHOW_COURSE_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\course.h"

namespace gui {
	namespace course_subscreens {
		class show_course : public screen {
		public:
			show_course();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void show();

			util::linked_list<course>::node_t *m_show = nullptr;
		};
	}
}

#endif