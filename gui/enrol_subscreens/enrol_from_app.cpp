#include "enrol_from_app.h"

#include "..\enrol.h"
#include "..\course_options.h"
#include "..\student_options.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"
#include "..\elements\checkbox.h"

namespace gui {
	namespace enrol_subscreens {
		enrol_from_app::enrol_from_app() {
			m_name = "From App Storage";
			setup();
		}

		void enrol_from_app::draw() {
			if (m_student_find) {
				get_element(3u)->show();
				get_element(4u)->show();

				if (m_course_find) {
					get_element(5u)->show();
					get_element(6u)->show();
				}
			} else {
				for (size_t i{ 3 }; i < m_elems.size(); ++i)
					get_element(i)->hide();
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void enrol_from_app::handle_event(sf::Event &evt) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void enrol_from_app::setup() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Find Student", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_student();
			}, true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button, true>("Find Course", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_course();
			}, true);

			y_offset += height + 10.f;
			add_gui_element<element::checkbox, true>("Student is retaking course", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button, true>("Enrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				enrol();
			}, true);
		}

		void enrol_from_app::find_student() {
			m_student_find = nullptr;
			m_course_find = nullptr;

			auto student_options = get_parent<enrol_options>()->get_student_options_subscr();
			if (!student_options) {
				g_app->push_warning("An unexpected error has occurred.");
				return;
			}

			auto students = &student_options->m_students;
			if (!students) {
				g_app->push_warning("An unexpected error has occurred.");
				return;
			}

			if (students->empty()) {
				g_app->push_warning("No student data is stored in the app!");
				return;
			}

			auto id = get_element<element::textbox>(1u)->get().toAnsiString();
			if (id.empty()) {
				g_app->push_warning("You must enter a student number.");
				return;
			}

			auto id32 = (u32)std::stoul(id, nullptr, 0);
			auto find = students->find_if([id32](student &s) {
				return s.student_number() == id32;
			});

			if (find == students->end()) {
				g_app->push_warning(std::string("Couldn't find student with student number ").append(id));
				return;
			}

			m_student_find = find;
			g_app->push_warning(std::string("Found student ").append(m_student_find->data.name()).append("."), 1.f, WARNING_SUCCESS);
		}

		void enrol_from_app::find_course() {
			if (!m_student_find)
				return;

			m_course_find = nullptr;

			auto course_options = get_parent<enrol_options>()->get_course_options_subscr();
			if (!course_options) {
				g_app->push_warning("An unexpected error has occurred.");
				return;
			}

			auto courses = &course_options->m_courses;
			if (!courses) {
				g_app->push_warning("An unexpected error has occurred.");
				return;
			}

			if (courses->empty()) {
				g_app->push_warning("No course data is stored in the app!");
				return;
			}

			auto id = get_element<element::textbox>(3u)->get().toAnsiString();
			if (id.empty()) {
				g_app->push_warning("You must enter a course id.");
				return;
			}

			auto id32 = (u32)std::stoul(id, nullptr, 0);
			auto find = courses->find_if([id32](course &c) {
				return c.id() == id32;
			});

			if (find == courses->end()) {
				g_app->push_warning(std::string("Couldn't find course with id ").append(id));
				return;
			}

			m_course_find = find;
			g_app->push_warning(std::string("Found course ").append(m_course_find->data.name()).append("."), 1.f, WARNING_SUCCESS);
		}

		void enrol_from_app::enrol() {
			if (!m_student_find || !m_course_find)
				return;

			auto enrolled = &get_parent<enrol_options>()->m_enrolled;
			if (!enrolled) {
				g_app->push_warning("An unexpected error occurred.");
				return;
			}

			auto is_retake = get_element<element::checkbox>(5u)->get();

			auto key = util::uint_pair<u64, u32, u32>(m_student_find->data.student_number(), m_course_find->data.id());
			student_on_course value(m_student_find->data, m_course_find->data, is_retake);

			enrolled->emplace(std::move(key), std::move(value));

			g_app->push_warning(std::string("Enrolled student ").append(m_student_find->data.name()).append(" on course ").append(m_course_find->data.name()), 1.f, WARNING_SUCCESS);
			m_student_find = nullptr;
			m_course_find = nullptr;
		}
	}
}