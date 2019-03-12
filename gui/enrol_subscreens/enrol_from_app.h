#ifndef ENROL_STUDENT_FROM_APP_H
#define ENROL_STUDENT_FROM_APP_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\student.h"
#include "..\..\course.h"

namespace gui {
	class enrol_options;

	namespace enrol_subscreens {
		class enrol_from_app : public screen {
		public:
			enrol_from_app();
			void draw() override;
			void handle_event(sf::Event &evt) override;

		private:
			void setup();
			void find_student();
			void find_course();
			void enrol();

		private:
			util::linked_list<student>::node_t *m_student_find = nullptr;
			util::linked_list<course>::node_t *m_course_find = nullptr;
		};
	}
}

#endif