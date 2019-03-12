#ifndef STUDENT_OPTIONS_AMMEND_STUDENT_H
#define STUDENT_OPTIONS_AMMEND_STUDENT_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\student.h"

namespace gui {
	namespace student_subscreens {
		class ammend_student : public screen {
		public:
			ammend_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void find_ammend_target();
			void ammend_target();
			util::linked_list<student>::node_t *m_ammend_target = nullptr;
		};
	}
}

#endif