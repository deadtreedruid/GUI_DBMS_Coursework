#ifndef MAIN_MENU_H
#define MAIN_MENU_H
#include "screen.h"

namespace gui {
	class enrol_options;

	namespace student_subscreens { class mark_assessment; }
	namespace course_subscreens { class add_assessment; }

	class main_menu final : public screen_subscreenable {
		friend class enrol_options;
		friend class student_subscreens::mark_assessment;
		friend class course_subscreens::add_assessment;
	public:
		main_menu();
		void draw() final;
		void handle_event(sf::Event &evt) final;
	private:
		void setup_controls();
		bool m_changed_themes = false;
		bool m_toggled_particles = false;
	};
} // namespace gui

#endif // MAIN_MENU_H
