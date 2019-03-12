#ifndef STUDENT_OPTIONS_VIEW_TRANSCRIPT_H
#define STUDENT_OPTIONS_VIEW_TRANSCRIPT_H
#include "..\screen.h"
#include "..\..\student_on_course.h"

namespace gui {
	namespace student_subscreens {
		class view_transcript : public screen {
		public:
			view_transcript();
			void draw() override;
			void handle_event(sf::Event &) override;

		private:
			void setup_controls();
			void find_student();
			void reset();
			inline auto next_free_elem() { return m_next_free_elem++; }

			template<assessment_type Type>
			void draw_assessment_set(student_on_course::progress_map<Type> &map, float &y_offset, const float x_pos, bool setup_frame = false);

			std::vector<student_on_course *> m_data;
			student *m_last = nullptr;
			size_t m_next_free_elem = 3u;
		};
	}
}

#endif