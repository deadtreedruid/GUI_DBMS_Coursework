#include "..\..\pugixml\src\pugixml.hpp"
#include "load_student.h"
#include "..\student_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		enum load_criteria : size_t {
			LOAD_ALL,
			LOAD_SPECIFIC
		};

		load_student::load_student() {
			m_name = "Load from File";
			setup_controls();
		}

		void load_student::draw() {
			static auto default_pos = get_element(4u)->pos();
			
			auto criteria = get_element<element::dropdown>(2u)->get();
			if (criteria == LOAD_ALL) {
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

		void load_student::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void load_student::setup_controls() {
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
			add_gui_element<element::dropdown>("Students to load", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Load all in file",
				"Load specific"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset -= spacing;
			y_offset += 40.f;
			add_gui_element<element::button>("Load", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				load();
			}, true);
		}

		load_student::list_type::node_t *load_student::load_from_node(pugi::xml_node &node, list_type *students, bool ignore_id_in_app_error) {
			std::string student_id{}, student_name{}, student_email{}, student_dob{}, student_enrol{};
			u32 sid_32{};
			for (auto &field : node.children()) {
				const auto &field_name = field.name();
				auto hashed_field_name = util::hash(field_name)();

				switch (hashed_field_name) {
				case util::hash::compile_time("StudentNumber"):
					student_id = field.text().as_string();
					sid_32 = field.text().as_uint();
					break;
				case util::hash::compile_time("Name"):
					student_name = field.text().as_string();
					break;
				case util::hash::compile_time("Email"):
					student_email = field.text().as_string();
					break;
				case util::hash::compile_time("DOB"):
					student_dob = field.text().as_string();
					break;
				case util::hash::compile_time("EnrolDate"):
					student_enrol = field.text().as_string();
					break;
				default: break;
				}
			}

			// check if student id already exists to prevent conflicts
			auto find = students->find_if([sid_32](student &s) {
				return sid_32 == s.student_number();
			});

			if (find != students->end()) {
				// hacky but this function can also be called from enrol_from_file where we don't want this error even if it is the case
				if (!ignore_id_in_app_error)
					g_app->push_warning(std::string("Student ").append(student_name).append(" in file has an id that already exists in the app!"), 1.f);
				return nullptr;
			}

			auto s = student::from_strings(student_id, student_name, student_email, student_dob, student_enrol);
			if (!s.is_valid_student()) {
				g_app->push_warning("An unexpected issue occured. Make sure the student file is formatted correctly.", 1.f);
				return nullptr;
			}

			students->push_back(
				std::move(s)
			);

			return students->back();
		}

		void load_student::load() {
			auto file_name = get_element<element::textbox>(1u)->get().toAnsiString();
			if (file_name.empty()) {
				g_app->push_warning("You need to enter a file name.");
				return;
			}

			if (file_name.find(".xml") == std::string::npos) {
				auto find_extension = file_name.find(".");
				if (find_extension != std::string::npos)
					file_name.erase(file_name.begin() + find_extension, file_name.end());

				file_name.append(".xml");
			}

			auto criteria = get_element<element::dropdown>(2u)->get();
			std::string target{};
			if (criteria == LOAD_SPECIFIC) {
				target = get_element<element::textbox>(3u)->get().toAnsiString();
				if (target.empty()) {
					g_app->push_warning("You need to enter a specific student number or change the criteria!", 1.f);
					return;
				}

				if (!util::string_util::is_number_string(target)) {
					g_app->push_warning("Entered student number must be a number");
					return;
				}
			}

			pugi::xml_document doc;
			if (auto result = doc.load_file(file_name.data()); result.status != pugi::status_ok) {
				g_app->push_warning(std::string("Failed to load file ").append(file_name).append("."));
				return;
			}

			auto students = &get_parent<student_options>()->m_students;
			if (!students) {
				g_app->push_warning("An unexpected issue occured.");
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

				if (criteria == LOAD_ALL) {
					auto result = load_from_node(student_node, students);
					if (result)
						at_least_one_added = true;
					continue;
				}

				bool match = false;
				for (auto &field : student_node.children()) {
					const auto &field_name = field.name();
					if (util::hash(field_name)() == util::hash::compile_time("StudentNumber")) {
						match = util::hash(field.text().as_string())() == util::hash(target)();
						if (match)
							break;
					} else {
						continue;
					}
				}

				if (match) {
					auto result = load_from_node(student_node, students);
					if (result) {
						g_app->push_warning(
							std::string("Successfully loaded student ").append(students->back()->data.name()).append(" with student number ").append(std::to_string(students->back()->data.student_number())),
							2.f,
							WARNING_SUCCESS
						);
					}
					return;
				}
			}

			if (at_least_one_added)
				g_app->push_warning(std::string("Added all students from file ").append(file_name), 0.5f, WARNING_SUCCESS);
		}
	}
}