#include "enrol.h"

#include "elements\button.h"
#include "elements\dropdown.h"
#include "elements\slider.h"
#include "elements\textbox.h"

#include "enrol_subscreens\enrol_from_app.h"
#include "enrol_subscreens\enrol_from_file.h"
#include "enrol_subscreens\unenrol_student.h"

#include "main_menu.h"
#include "course_options.h"
#include "student_options.h"

namespace gui {
	enrol_options::enrol_options() {
		setup();
	}

	void enrol_options::draw() {
		g_app->draw_string("Liverpool Hope University", 40, { 30.f, 15.f }, g_app->theme().text_color);

		if (!m_active_subscreen) {
			g_app->draw_string("Enrol Student On Course", 25, { 30.f, 70.f }, g_app->theme().text_color);

			for (auto elem : m_elems) {
				if (m_focus != elem)
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		} else {
			g_app->draw_string(std::string("Enrol Student On Course - ").append(m_active_subscreen->name()).data(), 25, { 30.f, 70.f }, g_app->theme().text_color);
			m_active_subscreen->draw();
		}
	}

	void enrol_options::handle_event(sf::Event &evt) {
		if (!m_active_subscreen) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		} else {
			m_active_subscreen->handle_event(evt);
		}
	}

	void enrol_options::setup() {
		add_subscreen<enrol_subscreens::enrol_from_app>();
		add_subscreen<enrol_subscreens::enrol_from_file>();
		add_subscreen<enrol_subscreens::unenrol_student>();

		constexpr float height = 30.f;
		constexpr float spacing = height + 10.f;

		const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
		const auto screen_y = (float)g_app->window()->getSize().y;

		add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
			this->get_parent<screen_subscreenable>()->to_root();
		});

		auto y_offset = screen_y * 0.35f;
		add_gui_element<element::button>("Enrol from App Data", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
			goto_subscreen(m_subscreens[0u]);
		}, true);

		y_offset += height + 10.f;
		add_gui_element<element::button>("Enrol from File", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
			goto_subscreen(m_subscreens[1u]);
		}, true);

		y_offset += height + 10.f;
		add_gui_element<element::button>("Unenrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
			goto_subscreen(m_subscreens[2u]);
		}, true);
	}

	course_options *enrol_options::get_course_options_subscr() {
		return get_parent<main_menu>()->get_subscreen<course_options>(1u);
	}

	student_options *enrol_options::get_student_options_subscr() {
		return get_parent<main_menu>()->get_subscreen<student_options>(0u);
	}
}