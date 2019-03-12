#include "student_options.h"

#include "elements\button.h"
#include "elements\dropdown.h"
#include "elements\slider.h"
#include "elements\textbox.h"

#include "student_subscreens\add_student.h"
#include "student_subscreens\show_student.h"
#include "student_subscreens\save_student.h"
#include "student_subscreens\load_student.h"
#include "student_subscreens\ammend_student.h"
#include "student_subscreens\mark_assessment.h"
#include "student_subscreens\view_transcript.h"

namespace gui {
	student_options::student_options() {
		setup();
	}

	void student_options::draw() {
		g_app->draw_string("Liverpool Hope University", 40, { 30.f, 15.f }, g_app->theme().text_color);

		if (!m_active_subscreen) {
			g_app->draw_string("Student Options", 25, { 30.f, 70.f }, g_app->theme().text_color);

			for (auto elem : m_elems) {
				if (m_focus != elem)
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		} else {
			g_app->draw_string(std::string("Student Options - ").append(m_active_subscreen->name()).data(), 25, { 30.f, 70.f }, g_app->theme().text_color);
			m_active_subscreen->draw();
		}
	}

	void student_options::handle_event(sf::Event &evt) {
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

	void student_options::setup() {
		add_subscreen<student_subscreens::add_student>();
		add_subscreen<student_subscreens::show_student>();
		add_subscreen<student_subscreens::save_student>();
		add_subscreen<student_subscreens::load_student>();
		add_subscreen<student_subscreens::ammend_student>();
		add_subscreen<student_subscreens::mark_assessment>();
		add_subscreen<student_subscreens::view_transcript>();

		constexpr float height = 30.f;
		constexpr float spacing = height + 10.f;

		const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
		const auto screen_y = (float)g_app->window()->getSize().y;

		add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
			this->get_parent<screen_subscreenable>()->to_root();
		});

		auto y_offset = screen_y * 0.35f;
		add_gui_element<element::button>("Add Student", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {			
			goto_subscreen(m_subscreens[0]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Show Student", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {		
			goto_subscreen(m_subscreens[1]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Save Student to File", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {	
			goto_subscreen(m_subscreens[2]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Load Student from File", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {		
			goto_subscreen(m_subscreens[3]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Ammend Student Details", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[4]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Mark Assessment", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[5]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("View Transcript", sf::Vector2f(center_x, y_offset), sf::Vector2f(210.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[6]);
		}, true);
	}
}