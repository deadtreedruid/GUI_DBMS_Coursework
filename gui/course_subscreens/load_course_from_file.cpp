#include "..\..\pugixml\src\pugixml.hpp"
#include "load_course_from_file.h"
#include "..\course_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		enum load_criteria : size_t {
			LOAD_ALL,
			LOAD_SPECIFIC
		};

		load_course_from_file::load_course_from_file() {
			m_name = "Load from File";
			setup_controls();
		}

		void load_course_from_file::draw() {
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

		void load_course_from_file::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void load_course_from_file::setup_controls() {
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
			add_gui_element<element::dropdown>("Courses to load", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Load all in file",
				"Load specific"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset -= spacing;
			y_offset += 40.f;
			add_gui_element<element::button>("Load", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				load();
			}, true);
		}

		load_course_from_file::list_type::node_t *load_course_from_file::load_from_node(pugi::xml_node &node, list_type *courses) {
			std::string course_id{}, course_name{}, course_begin{}, course_end{}, course_level{}, course_credits{};
			u32 cid_32{};
			for (auto &field : node.children()) {
				const auto &field_name = field.name();
				auto hashed_field_name = util::hash(field_name)();

				switch (hashed_field_name) {
				case util::hash::compile_time("ID"):
					course_id = field.text().as_string();
					cid_32 = (u32)field.text().as_uint();
					break;
				case util::hash::compile_time("Name"):
					course_name = field.text().as_string();
					break;
				case util::hash::compile_time("StartDate"):
					course_begin = field.text().as_string();
					break;
				case util::hash::compile_time("EndDate"):
					course_end = field.text().as_string();
					break;
				case util::hash::compile_time("Level"):
					course_level = field.text().as_string();
					break;
				case util::hash::compile_time("Credits"):
					course_credits = field.text().as_string();
					break;
				default: break;
				}
			}

			// check if student id already exists to prevent conflicts
			auto find = courses->find_if([cid_32](course &s) {
				return cid_32 == s.id();
			});

			if (find != courses->end()) {
				g_app->push_warning(std::string("Course ").append(course_name).append(" in file has an id that already exists in the app!"), 1.5f);
				return nullptr;
			}

			auto c = course::from_strings(course_id, course_name, course_begin, course_end, course_level, course_credits);
			if (!c.valid_course()) {
				g_app->push_warning(std::string("Tried to load invalid course with name ").append(course_name));
				return nullptr;
			}

			courses->push_back(
				std::move(c)
			);
			
			// even though we've moved it, c's destructor will still be called once its out of scope so we want to make the assessment sets in place
			courses->back()->data.make_assessment_sets();

			return courses->back();
		}

		void load_course_from_file::load() {
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
					g_app->push_warning("You need to enter a specific course ID or change the criteria!", 2.f);
					return;
				}

				if (!std::all_of(target.begin(), target.end(), [](char c) { return isdigit(c); })) {
					g_app->push_warning("Entered course ID must be a number");
					return;
				}
			}

			pugi::xml_document doc;
			if (auto result = doc.load_file(file_name.data()); result.status != pugi::status_ok) {
				g_app->push_warning(std::string("Failed to load file ").append(file_name).append("."));
				return;
			}

			auto courses = &get_parent<course_options>()->m_courses;
			if (!courses) {
			#ifdef _DEBUG
				printf("load_course_from_file::load() - parent course details are nullptr");
			#endif
				return;
			}

			auto courses_node = doc.child("Courses");
			if (!courses_node) {
				g_app->push_warning("File does not contain any courses.");
				return;
			}

			bool at_least_one_added = false;
			for (auto &course_node : courses_node.children()) {
				if (!course_node)
					continue;

				if (criteria == LOAD_ALL) {
					auto result = load_from_node(course_node, courses);
					if (result) 
						at_least_one_added = true;
					continue;
				}

				bool match = false;
				for (auto &field : course_node.children()) {
					const auto &field_name = field.name();
					if (util::hash(field_name)() == util::hash::compile_time("ID")) {
						match = util::hash(field.text().as_string())() == util::hash(target)();
						if (match)
							break;
					} else {
						continue;
					}
				}

				if (match) {
					auto result = load_from_node(course_node, courses);
					if (result) {
						g_app->push_warning(
							std::string("Successfully loaded course ").append(courses->back()->data.name()).append(" with id ").append(std::to_string(courses->back()->data.id())),
							2.f,
							WARNING_SUCCESS
						);
					}
					return;
				}
			}

			if (at_least_one_added)
				g_app->push_warning(std::string("Added all courses from file ").append(file_name), 1.f, WARNING_SUCCESS);
		}
	}
}