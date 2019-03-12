#include "show_student.h"
#include "..\student_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		enum show_criteria : size_t {
			CRITERIA_STUDENTNUM,
			CRITERIA_NAME
		};

		show_student::show_student() {
			m_name = "Show Student";
			setup_controls();
		}

		void show_student::draw() {
			auto criteria = get_element<element::dropdown>(1u)->get();
			if (criteria == CRITERIA_STUDENTNUM) {
				get_element(3u)->hide();
				get_element(2u)->show();			
			} else {
				get_element(2u)->hide();
				get_element(3u)->show();
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();

			if (m_show) {
				static std::string student_id, student_name, student_email, student_dob, student_enrol;
				static decltype(m_show) last_shown = nullptr;
				if (m_show != last_shown && m_show->data.is_valid_student()) {
					student_id = std::string("Student Number: ").append(std::to_string(m_show->data.student_number()));
					student_name = std::string("Name: ").append(m_show->data.name());
					student_email = std::string("Email: ").append(m_show->data.email());
					student_dob = std::string("Date of Birth: ").append(m_show->data.date_of_birth().to_string());
					student_enrol = std::string("Enrol Date: ").append(m_show->data.enrollment_date().to_string());
					last_shown = m_show;
				}

				if (!student_id.empty() && !student_name.empty() && !student_email.empty() && !student_dob.empty() && !student_enrol.empty()) {
					static const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
					static const auto screen_y = (float)g_app->window()->getSize().y;

					sf::Vector2f rect_pos, rect_size;
					rect_pos.y = (screen_y * 0.3f) + 120.f + 60.f;
					rect_pos.x = center_x;

					auto max_x = std::numeric_limits<float>::min();
					auto y_add = 10.f;
					rect_size.y += (y_add * 2.f);

					auto draw_student_str = [&y_add, &max_x, rect_pos](const std::string &str, bool increment_y = true) {
						g_app->draw_string(str.data(), g_app->theme().text_size, sf::Vector2f(center_x, rect_pos.y + y_add), g_app->theme().text_color, true);
						auto bounds = g_app->string_bounds(str.data(), g_app->theme().text_size);

						if (increment_y)
							y_add += bounds.y + 10.f;

						max_x = std::max(max_x, bounds.x + 20.f);
					};

					draw_student_str(student_id);
					draw_student_str(student_name);
					draw_student_str(student_email);
					draw_student_str(student_dob);
					draw_student_str(student_enrol, false);
					
					rect_size.x = max_x;
					rect_size.y += y_add;
					rect_pos.x = center_x - (rect_size.x * 0.5f);
					g_app->draw_rect_outline(rect_pos, rect_size, g_app->theme().accent_color);
				}
			}
		}

		void show_student::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void show_student::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::dropdown>("Criteria", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Student Number",
				"Student Name"
			}),
			true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);
			add_gui_element<element::textbox, true>("Student Name", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += 40.f;
			add_gui_element<element::button>("Show", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				show();
			}, true);
		}

		void show_student::show() {
			m_show = nullptr;

			auto criteria = get_element<element::dropdown>(1u)->get();
			auto target = [this, criteria]() {
				if (criteria == CRITERIA_STUDENTNUM)
					return this->get_element<element::textbox>(2u)->get().toAnsiString();
				else
					return this->get_element<element::textbox>(3u)->get().toAnsiString();
			}();

			if (target.empty()) {
				g_app->push_warning("Target is empty so there's nothing to show.");
				return;
			}

			get_element<element::textbox>(2u)->set("");
			get_element<element::textbox>(3u)->set("");

			auto students = &get_parent<student_options>()->m_students;
			if (!students) {
				g_app->push_warning("An unexpected error occured.");
				return;
			}

			if (students->empty()) {
				g_app->push_warning("This program currently holds no student records.");
				return;
			}

			u32 target_32{};
			util::hash::hash_t target_hash{};
			if (criteria == CRITERIA_STUDENTNUM)
				target_32 = (u32)std::stoul(target, nullptr, 0);
			else
				target_hash = util::hash(target);

			auto find = students->find_if([criteria, target_32, target_hash](student &s) {
				if (criteria == CRITERIA_STUDENTNUM)
					return target_32 == s.student_number();
				else
					return target_hash == util::hash(s.name())();
			});

			if (find == students->end()) {
				g_app->push_warning("Couldn't find a student matching your criteria.");
				return;
			} else {
				if (!find->data.is_valid_student()) {
					g_app->push_warning("Found a student that matches the criteria but it isn't valid, try ammending its details.", 1.5f);
					return;
				}

				m_show = find;
				g_app->push_warning("Found student!", 1.f, WARNING_SUCCESS);
			}
		}
	}
}