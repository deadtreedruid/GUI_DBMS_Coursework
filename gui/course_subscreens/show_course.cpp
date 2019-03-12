#include "show_course.h"
#include "..\course_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		enum show_criteria : size_t {
			CRITERIA_COURSEID,
			CRITERIA_NAME
		};

		show_course::show_course() {
			m_name = "Show Course";
			setup_controls();
		}

		void show_course::draw() {
			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();

			if (m_show) {
				static std::string course_id, course_name, course_start, course_end, course_level, course_credit;
				static decltype(m_show) last_shown = nullptr;
				if (m_show != last_shown) {
					course_id = std::string("ID: ").append(std::to_string(m_show->data.id()));
					course_name = std::string("Name: ").append(m_show->data.name());
					course_start = std::string("Start Date: ").append(m_show->data.start_date().to_string());
					course_end = std::string("End Date: ").append(m_show->data.end_date().to_string());
					course_level = std::string("Level: ").append(m_show->data.level_as_string());
					course_credit = std::string("Credits: ").append(m_show->data.credit_as_string());
					last_shown = m_show;
				}

				if (!course_id.empty() && !course_name.empty() && !course_start.empty() && !course_end.empty() && !course_level.empty() && !course_credit.empty()) {
					static const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
					static const auto screen_y = (float)g_app->window()->getSize().y;

					sf::Vector2f rect_pos, rect_size;
					rect_pos.y = (screen_y * 0.3f) + 120.f + 60.f;
					rect_pos.x = center_x;

					auto max_x = std::numeric_limits<float>::min();
					auto y_add = 10.f;
					rect_size.y += (y_add * 2.f);

					auto draw_course_str = [&y_add, &max_x, rect_pos](const std::string &str, bool increment_y = true) {
						g_app->draw_string(str.data(), g_app->theme().text_size, sf::Vector2f(center_x, rect_pos.y + y_add), g_app->theme().text_color, true);
						auto bounds = g_app->string_bounds(str.data(), g_app->theme().text_size);

						if (increment_y)
							y_add += bounds.y + 10.f;

						max_x = std::max(max_x, bounds.x + 20.f);
					};

					draw_course_str(course_id);
					draw_course_str(course_name);
					draw_course_str(course_start);
					draw_course_str(course_end);
					draw_course_str(course_level);
					draw_course_str(course_credit, false);
					
					rect_size.x = max_x;
					rect_size.y += y_add;
					rect_pos.x = center_x - (rect_size.x * 0.5f);
					g_app->draw_rect_outline(rect_pos, rect_size, g_app->theme().accent_color);
				}
			}
		}

		void show_course::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void show_course::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::dropdown>("Criteria", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Course ID",
				"Course Name"
			}),
			true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Target", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += 40.f;
			add_gui_element<element::button>("Show", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				show();
			}, true);
		}

		void show_course::show() {
			m_show = nullptr;

			auto criteria = get_element<element::dropdown>(1u)->get();
			auto target = get_element<element::textbox>(2u)->get().toAnsiString();
			if (target.empty()) {
				g_app->push_warning("Target is empty so there's nothing to show.");
				return;
			}

			auto courses = &get_parent<course_options>()->m_courses;
			if (courses->empty()) {
				g_app->push_warning("This program currently holds no course records.");
				return;
			}

			u32 target_32{};
			util::hash::hash_t target_hash{};
			if (criteria == CRITERIA_COURSEID)
				target_32 = (u32)std::stoul(target, nullptr, 0);
			else
				target_hash = util::hash(target);

			auto find = courses->find_if([criteria, target_32, target_hash](course &c) {
				if (criteria == CRITERIA_COURSEID)
					return target_32 == c.id();
				else
					return target_hash == util::hash(c.name())();
			});

			if (find == courses->end()) {
				g_app->push_warning("Couldn't find a course matching your criteria.");
				return;
			} else {
				if (!find->data.valid_course()) {
					g_app->push_warning("Found a course that matches the criteria but it isn't valid, try ammending its details.", 2.f);
					return;
				}

				m_show = find;
				g_app->push_warning("Found course!", 1.5f, WARNING_SUCCESS);
			}
		}
	}
}