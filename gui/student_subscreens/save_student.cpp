#include "..\..\pugixml\src\pugixml.hpp"
#include "save_student.h"
#include "..\student_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		enum save_criteria : size_t {
			SAVE_ALL,
			SAVE_SPECIFIC
		};

		save_student::save_student() {
			m_name = "Save to File";
			setup_controls();
		}

		void save_student::draw() {
			static auto default_pos = get_element(4u)->pos();

			auto criteria = get_element<element::dropdown>(2u)->get();
			if (criteria == SAVE_ALL) {
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

		void save_student::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void save_student::setup_controls() {
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
			add_gui_element<element::dropdown>("Students to save", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Save all stored",
				"Save specific"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset -= spacing;
			y_offset += 40.f;
			add_gui_element<element::button>("Save", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				save();
			}, true);
		}

		void save_student::save() {
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
			if (criteria == SAVE_SPECIFIC) {
				target = get_element<element::textbox>(3u)->get().toAnsiString();
				if (target.empty()) {
					g_app->push_warning("You need to enter a specific student ID or change the criteria!", 1.5f);
					return;
				}

				if (!util::string_util::is_number_string(target)) {
					g_app->push_warning("Entered student ID must be a number");
					return;
				}
			}

			pugi::xml_document doc;
			auto root = doc.append_child("Students");

			auto save_course = [&root](decltype(student_options::m_students)::node_t *node) {
				auto course_node = root.append_child("Student");

				auto id_node = course_node.append_child("StudentNumber");
				id_node.append_child(pugi::node_pcdata).set_value(std::to_string(node->data.student_number()).data());

				auto name_node = course_node.append_child("Name");
				name_node.append_child(pugi::node_pcdata).set_value(node->data.name().data());

				auto email_node = course_node.append_child("Email");
				email_node.append_child(pugi::node_pcdata).set_value(node->data.email().data());

				auto dob_node = course_node.append_child("DOB");
				dob_node.append_child(pugi::node_pcdata).set_value(node->data.date_of_birth().to_string().data());

				auto enrol_node = course_node.append_child("EnrolDate");
				enrol_node.append_child(pugi::node_pcdata).set_value(node->data.enrollment_date().to_string().data());
			};

			auto students = &get_parent<student_options>()->m_students;
			if (!students) {
				g_app->push_warning("An unexpected error occured.");
				return;
			}

			if (students->empty()) {
				g_app->push_warning("No student data currently stored.");
				return;
			}

			if (criteria == SAVE_SPECIFIC) {
				auto target_32 = (u32)std::stoul(target, nullptr, 0);
				auto find = students->find_if([target_32](student &s) {
					return s.student_number() == target_32;
				});

				if (find == students->end()) {
					g_app->push_warning(std::string("Couldn't find student with student number ").append(target));
					return;
				}

				save_course(find);
			} else {
				for (auto it = students->begin(); it != students->end(); ++it) {
					if (!it)
						continue;

					save_course(it);
				}
			}

			if (!doc.save_file(file_name.data()))
				g_app->push_warning("Failed to save file, make sure you have permission to write files to this folder!", 1.7f);
			else
				g_app->push_warning(std::string("Successfully saved file ").append(file_name), 1.f, WARNING_SUCCESS);
			
		}
	}
}