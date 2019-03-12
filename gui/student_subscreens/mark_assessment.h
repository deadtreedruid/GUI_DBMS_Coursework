#ifndef STUDENT_OPTIONS_MARK_ASSESSMENT_H
#define STUDENT_OPTIONS_MARK_ASSESSMENT_H
#include "..\screen.h"
#include "..\..\student_on_course.h"
#include "..\elements\dropdown.h"

namespace gui {
	namespace student_subscreens {
		class mark_assessment : public screen {
		public:
			mark_assessment();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void update_mark();
			void find_data();

			template<assessment_type Type>
			static void populate_assessments(const student_on_course::progress_map<Type> &map, element::dropdown *gui_elem);

			student_on_course *m_data = nullptr;
		};
	}
}

#endif