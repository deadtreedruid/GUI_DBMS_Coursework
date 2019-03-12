#ifndef COURSE_OPTIONS_H
#define COURSE_OPTIONS_H
#include "screen.h"
#include "..\course.h"
#include "..\util\linked_list.h"

namespace gui {
	namespace course_subscreens { class add_assessment; }

	class course_options : public screen_subscreenable {
		friend class course_subscreens::add_assessment;
	public:
		// add subscreens here
		course_options();

		void draw() override;
		void handle_event(sf::Event &evt) override;
		util::linked_list<course> m_courses;
	private:
		void setup();
	};
}

#endif