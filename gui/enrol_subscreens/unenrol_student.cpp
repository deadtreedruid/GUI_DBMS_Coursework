#include "..\..\pugixml\src\pugixml.hpp"
#include "unenrol_student.h"

#include "..\enrol.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace enrol_subscreens {
		enum unenrol_criteria : size_t {
			UNENROL_ALL,
			UNENROL_SPECIFIC
		};

		unenrol_student::unenrol_student() {
			m_name = "Unenrol Student";
			setup_controls();
		}

		void unenrol_student::draw() {
			static auto default_pos = get_element(4u)->pos();

			auto criteria = get_element<element::dropdown>(2u)->get();
			if (criteria == UNENROL_ALL) {
				get_element(3u)->hide();
				get_element(4u)->reposition(default_pos);
			} else {
				get_element(3u)->show();
				get_element(4u)->reposition({ default_pos.x, default_pos.y + 60.f });
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void unenrol_student::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void unenrol_student::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::dropdown>("Who to unenrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"All Students on Course",
				"Specific student"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset -= spacing;
			y_offset += height + 10.f;
			add_gui_element<element::button>("Unenrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				unenrol();
			}, true);
		}

		void unenrol_student::unenrol() {
			auto course_id = get_element<element::textbox>(1u)->get().toAnsiString();
			if (course_id.empty()) {
				g_app->push_warning("You must enter a course ID.");
				return;
			}

			if (!util::string_util::is_number_string(course_id)) {
				g_app->push_warning("Course ID must be a number.");
				return;
			}

			auto course_32 = (u32)std::stoul(course_id, nullptr, 0);
			
			auto criteria = get_element<element::dropdown>(2u)->get();
			std::string student_number{};
			u32 student_32;
			if (criteria == UNENROL_SPECIFIC) {
				student_number = get_element<element::textbox>(3u)->get().toAnsiString();
				if (student_number.empty()) {
					g_app->push_warning("You must enter a student number or change the criteria.");
					return;
				}

				if (!util::string_util::is_number_string(student_number)) {
					g_app->push_warning("Student number must be a number.");
					return;
				}

				student_32 = (u32)std::stoul(student_number, nullptr, 0);
			}

			auto enrolled = &get_parent<enrol_options>()->m_enrolled;
			if (!enrolled) {
				g_app->push_warning("An unexpected error occurred.");
				return;
			}

			if (enrolled->empty()) {
				g_app->push_warning("No students are currently enrolled on any courses.");
				return;
			}

			// find each course instance
			auto begin = enrolled->begin();
			auto end = enrolled->end();
			std::vector<enrol_options::key_type> matches{};

			// can't use std::count_if or anything to avoid student_on_course object being destructed when we dont want it to
			size_t count{};
			for (auto it = begin; it != end; ++it) {
				auto k = &it->first;

				// high order bits are the course id
				if (k->hibits() == course_32) {
					// low order bits are student number
					// only push matches to container
					if (criteria == UNENROL_ALL || (criteria == UNENROL_SPECIFIC && k->lobits() == student_32))
						matches.emplace_back(*k);

					++count;
				}
			}

			if (matches.empty()) {
				if (!count)
					g_app->push_warning(std::string("App hold no data on course with id ").append(course_id));
				else
					g_app->push_warning(std::string("Student ").append(student_number).append(" isn't enrolled on course ").append(course_id));
				return;
			}

			for (const auto &match : matches)
				enrolled->erase(match);

			g_app->push_warning(criteria == UNENROL_SPECIFIC ? 
				std::string("Unenrolled student ").append(student_number).append("!") : 
				std::string("Unenrolled all students from course ").append(course_id), 
				1.f, WARNING_SUCCESS
			);
		}
	}
}