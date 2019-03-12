#ifndef STUDENT_OPTIONS_H
#define STUDENT_OPTIONS_H
#include "screen.h"
#include "..\student.h"
#include "..\util\linked_list.h"

namespace gui {
	namespace student_subscreens {
		class mark_assessment;
	}

	class student_options : public screen_subscreenable {
		friend class student_subscreens::mark_assessment;
	public:
		// add subscreens here
		student_options();

		void draw() override;
		void handle_event(sf::Event &evt) override;
		util::linked_list<student> m_students;
	private:
		void setup();
	};
}

#endif