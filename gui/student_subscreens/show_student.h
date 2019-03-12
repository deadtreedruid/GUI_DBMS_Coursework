#ifndef STUDENT_OPTIONS_SHOW_STUDENT_H
#define STUDENT_OPTIONS_SHOW_STUDENT_H
#include "..\screen.h"
#include "..\..\util\linked_list.h"
#include "..\..\student.h"

namespace gui {
	namespace student_subscreens {
		class show_student : public screen {
		public:
			show_student();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void show();

			util::linked_list<student>::node_t *m_show = nullptr;
		};
	}
}

#endif