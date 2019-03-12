#include "..\..\pugixml\src\pugixml.hpp"
#include "enrol_from_file.h"

#include "..\enrol.h"
#include "..\student_options.h"
#include "..\course_options.h"
#include "..\student_subscreens\load_student.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"
#include "..\elements\checkbox.h"

namespace gui {
	namespace enrol_subscreens {
		enum enrol_criteria : size_t {
			ENROL_ALL,
			ENROL_SPECIFIC
		};

		enrol_from_file::enrol_from_file() {
			m_name = "Enrol from File";
			setup_controls();
		}

		void enrol_from_file::draw() {
			static auto default_pos = get_element(5u)->pos();

			static size_t last_criteria = std::numeric_limits<size_t>::max();
			auto criteria = get_element<element::dropdown>(3u)->get();
			if (criteria != last_criteria) {
				if (criteria == ENROL_ALL) {
					get_element(4u)->hide();
					get_element(5u)->reposition(default_pos);
					get_element(6u)->reposition({ default_pos.x, default_pos.y + 40.f });
				} else {
					get_element(4u)->show();
					get_element(5u)->reposition({ default_pos.x, default_pos.y + 60.f });
					get_element(6u)->reposition({ default_pos.x, default_pos.y + 60.f + 40.f });
				}
				last_criteria = criteria;
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void enrol_from_file::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void enrol_from_file::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Filename", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::dropdown>("Students to Enrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Enrol all in file",
				"Enrol specific"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset -= spacing;
			y_offset += height + 10.f;
			add_gui_element<element::checkbox>("Student(s) re-taking course", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Enrol", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				enrol();
			}, true);
		}

		void enrol_from_file::enrol() {
			auto courses = &get_parent<enrol_options>()->get_course_options_subscr()->m_courses;
			auto students = &get_parent<enrol_options>()->get_student_options_subscr()->m_students;
			auto enrolled = &get_parent<enrol_options>()->m_enrolled;
			if (!courses || !students || !enrolled) {
				g_app->push_warning("An unexpected error occurred.");
				return;
			}

			if (courses->empty()) {
				g_app->push_warning("App currently has no course info stored.");
				return;
			}

			auto course_id = get_element<element::textbox>(2u)->get().toAnsiString();
			if (course_id.empty()) {
				g_app->push_warning("You must enter a course's id.");
				return;
			}

			auto course_32 = (u32)std::stoul(course_id, nullptr, 0);
			auto find = courses->find_if([course_32](course &c) {
				return c.id() == course_32;
			});

			if (find == courses->end()) {
				g_app->push_warning(std::string("Course with id ").append(course_id).append(" does not exist."));
				return;
			}
			
			auto criteria = get_element<element::dropdown>(3u)->get();
			std::string student_number{};
			u32 student_num32{};
			if (criteria == ENROL_SPECIFIC) {
				student_number = get_element<element::textbox>(4u)->get().toAnsiString();
				if (student_number.empty()) {
					g_app->push_warning("You need to enter a specific student number or change the criteria!", 0.8f);
					return;
				}

				if (!util::string_util::is_number_string(student_number)) {
					g_app->push_warning("Entered student number must be a number");
					return;
				}

				student_num32 = (u32)std::stoul(student_number, nullptr, 0);
			}

			auto file_name = get_element<element::textbox>(1u)->get().toAnsiString();
			if (file_name.empty()) {
				g_app->push_warning("You must enter a file name.");
				return;
			}

			auto is_retake = get_element<element::checkbox>(5u)->get();

			if (file_name.find(".xml") == std::string::npos) {
				auto find_extension = file_name.find(".");
				if (find_extension != std::string::npos)
					file_name.erase(file_name.begin() + find_extension, file_name.end());

				file_name.append(".xml");
			}

			pugi::xml_document doc;
			if (auto result = doc.load_file(file_name.data()); result.status != pugi::status_ok) {
				g_app->push_warning(std::string("Failed to load file ").append(file_name).append("."));
				return;
			}

			auto students_node = doc.child("Students");
			if (!students_node) {
				g_app->push_warning("File does not contain any students.");
				return;
			}

			bool at_least_one_added = false;
			for (auto &student_node : students_node.children()) {
				if (!student_node)
					continue;

				bool should_enrol = true;
				std::string student_name{};
				for (auto &field : student_node.children()) {
					// check that this is the target student
					if (util::hash(field.name()) == util::hash::compile_time("StudentNumber") && criteria == ENROL_SPECIFIC) {
						if (field.text().as_uint() != student_num32)
							should_enrol = false;
					} else if (util::hash(field.name()) == util::hash::compile_time("Name")) {
						// store this off for warning
						student_name = field.text().as_string();
					}
				}

				if (!should_enrol)
					continue;

				// we need to add them to our app's student database so we can have a student pointer
				// this can return a nullptr if that student already exists OR if the student node has bad data
				auto student_ptr = student_subscreens::load_student::load_from_node(student_node, students);
				if (!student_ptr) {
					// try and find this student within our app's data
					student_ptr = students->find_if([student_num32](student &s) {
						return s.student_number() == student_num32;
					});

					// still couldn't find the student
					if (student_ptr == students->end())
						continue;
				}

				// check that student isn't already enrolled on the specified course course
				auto key = util::uint_pair<u64, u32, u32>(student_num32, course_32);
				if (enrolled->find(key) != enrolled->end()) {
					g_app->push_warning(std::string("Student ").append(student_name).append(" already enrolled on course ").append(find->data.name()).append("."));
					continue;
				}

				student_on_course value{ student_ptr->data, find->data, is_retake };
				enrolled->emplace(std::move(key), std::move(value));
				at_least_one_added = true;
			}

			if (at_least_one_added)
				g_app->push_warning(std::string(criteria == ENROL_SPECIFIC ? "Enrolled specified student " : "Enrolled all students in file ").append("to course ").append(find->data.name()).append("."), 1.f, WARNING_SUCCESS);
		}
	}
}