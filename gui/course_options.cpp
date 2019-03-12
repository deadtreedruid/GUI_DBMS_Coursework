#include "course_options.h"

#include "elements\button.h"
#include "elements\dropdown.h"
#include "elements\slider.h"
#include "elements\textbox.h"

#include "course_subscreens\add_course.h"
#include "course_subscreens\show_course.h"
#include "course_subscreens\save_course_to_file.h"
#include "course_subscreens\load_course_from_file.h"
#include "course_subscreens\ammend_course.h"
#include "course_subscreens\add_assessment.h"
#include "course_subscreens\ammend_assessment.h"

namespace gui {
	course_options::course_options() {
		setup();
	}

	void course_options::draw() {
		g_app->draw_string("Liverpool Hope University", 40, { 30.f, 15.f }, g_app->theme().text_color);

		if (!m_active_subscreen) {
			g_app->draw_string("Course Options", 25, { 30.f, 70.f }, g_app->theme().text_color);

			for (auto elem : m_elems) {
				if (m_focus != elem)
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		} else {
			g_app->draw_string(std::string("Course Options - ").append(m_active_subscreen->name()).data(), 25, { 30.f, 70.f }, g_app->theme().text_color);
			m_active_subscreen->draw();
		}
	}

	void course_options::handle_event(sf::Event &evt) {
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

	void course_options::setup() {
		add_subscreen<course_subscreens::add_course>();
		add_subscreen<course_subscreens::show_course>();
		add_subscreen<course_subscreens::save_course_to_file>();
		add_subscreen<course_subscreens::load_course_from_file>();
		add_subscreen<course_subscreens::ammend_course>();
		add_subscreen<course_subscreens::add_assessment>();
		add_subscreen<course_subscreens::ammend_assessment>();

		constexpr float height = 30.f;
		constexpr float spacing = height + 10.f;

		const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
		const auto screen_y = (float)g_app->window()->getSize().y;

		add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
			this->get_parent<screen_subscreenable>()->to_root();
		});

		auto y_offset = screen_y * 0.27f;
		add_gui_element<element::button>("Add Course", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {			
			goto_subscreen(m_subscreens[0]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Show Course", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {		
			goto_subscreen(m_subscreens[1]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Save Course to File", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {	
			goto_subscreen(m_subscreens[2]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Load from File", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {		
			goto_subscreen(m_subscreens[3]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Ammend Course Details", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[4]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Add Assessment", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[5]);
		}, true);

		y_offset += spacing;
		add_gui_element<element::button>("Ammend Assessment Details", sf::Vector2f(center_x, y_offset), sf::Vector2f(250.f, height), [this](element::button *) {
			goto_subscreen(m_subscreens[6]);
		}, true);
	}
}