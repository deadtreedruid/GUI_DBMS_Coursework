#include "ammend_course.h"
#include "..\course_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		enum ammend_criteria : size_t {
			AMMEND_ID,
			AMMEND_NAME,
			AMMEND_STARTDATE,
			AMMEND_ENDDATE,
			AMMEND_LEVEL,
			AMMEND_CREDITS,
			AMMEND_INVALID
		};

		ammend_course::ammend_course() {
			m_name = "Ammend Course Details";
			setup_controls();
		}

		void ammend_course::draw() {
			auto category = get_element<element::dropdown>(3u);
			static auto last_category = std::numeric_limits<size_t>::max();
			if (category->get() != last_category) {
				get_element<element::textbox>(4u)->set("");
				get_element<element::dropdown>(5u)->set(0u);
				get_element<element::dropdown>(6u)->set(0u);
				last_category = category->get();
			}

			auto alter_ammend_elems = [this, &category](bool hide = true) {
				if (hide) {
					for (size_t i{ 3 }; i <= 7u; ++i)
						get_element(i)->hide();
				} else {
					category->show();

					for (size_t i{ 4u }; i <= 6u; ++i)
						get_element(i)->hide();

					// display different "New value" elements
					switch (category->get()) {
					case AMMEND_LEVEL:
						get_element(5u)->show();
						break;
					case AMMEND_CREDITS:
						get_element(6u)->show();
						break;
					case AMMEND_ID:
					case AMMEND_NAME:
					case AMMEND_STARTDATE:
					case AMMEND_ENDDATE:
						get_element(4u)->show();
						break;
					default: break;
					}

					get_element(7u)->show();
				}
			};

			alter_ammend_elems(m_ammend_target == nullptr);

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void ammend_course::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void ammend_course::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing - (float)g_app->theme().text_size;
			add_gui_element<element::button>("Find", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_ammend_target();
			}, true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Ammend Category", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"ID",
				"Name",
				"Start Date",
				"End Date",
				"Level",
				"Credits"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("New value", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);
			// level
			add_gui_element<element::dropdown, true>("New value", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Level C",
				"Level I",
				"Level H",
				"Level M"
			}), true);
			// credits
			add_gui_element<element::dropdown, true>("New value", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"0",
				"15",
				"30",
				"45",
				"60",
				"90",
				"120"
			}), true);


			y_offset += spacing - (float)g_app->theme().text_size;
			add_gui_element<element::button, true>("Ammend", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				ammend_target();
			}, true);
		}

		void ammend_course::find_ammend_target() {
			m_ammend_target = nullptr;

			auto courses = &get_parent<course_options>()->m_courses;
			if (!courses) {
			#ifdef _DEBUG
				printf("load_course_from_file::load() - parent course details are nullptr");
			#endif
				return;
			}

			auto id = get_element<element::textbox>(1u)->get().toAnsiString();
			if (id.empty()) {
				g_app->push_warning("Please enter a course id to find.", 1.5f);
				return;
			}

			auto search_id = (u32)std::stoul(id, nullptr, 0);
			auto find = courses->find_if([search_id](course &c) {
				return search_id == c.id();
			});

			if (find == courses->end()) {
				g_app->push_warning(std::string("Couldn't find course with id ").append(id), 1.5f);
				return;
			}

			m_ammend_target = find;
		}

		void ammend_course::ammend_target() {
			if (!m_ammend_target)
				return;

			auto criteria = get_element<element::dropdown>(3u)->get();
			if (criteria == AMMEND_LEVEL) {
				auto new_value = (u8)get_element<element::dropdown>(5u)->get();
				m_ammend_target->data.m_level = (course::course_level)new_value;
				g_app->push_warning("Ammended level!", 1.f, WARNING_SUCCESS);
			} else if (criteria == AMMEND_CREDITS) {
				auto new_value = (u8)get_element<element::dropdown>(6u)->get();
				m_ammend_target->data.m_credit = (course::credit_bearing)new_value;
				g_app->push_warning("Ammended credit bearing!", 1.f, WARNING_SUCCESS);
			} else {
				// use textbox value
				auto new_value = get_element<element::textbox>(4u)->get().toAnsiString();
				if (new_value.empty()) {
					g_app->push_warning("You need to enter a value to ammend!", 1.5f);
					return;
				}

				switch (criteria) {
				case AMMEND_NAME:
					m_ammend_target->data.m_name = new_value;
					g_app->push_warning("Ammended name!", 1.f, WARNING_SUCCESS);
					break;
				case AMMEND_STARTDATE:
					if (!util::is_valid_date_string(new_value)) {
						g_app->push_warning("New value must be valid date following XX/XX/YYYY format!", 1.5f);
						return;
					}

					m_ammend_target->data.m_begin = util::date_time(new_value);
					g_app->push_warning("Ammended start date!", 1.f, WARNING_SUCCESS);
					break;
				case AMMEND_ENDDATE:
					if (!util::is_valid_date_string(new_value)) {
						g_app->push_warning("New value must be valid date following XX/XX/YYYY format!", 1.5f);
						return;
					}

					m_ammend_target->data.m_end = util::date_time(new_value);
					g_app->push_warning("Ammended end date!", 1.f, WARNING_SUCCESS);
					break;
				case AMMEND_ID:
					if (!util::string_util::is_number_string(new_value)) {
						g_app->push_warning("New value must be a number!");
						return;
					}

					m_ammend_target->data.m_id = (u32)std::stoul(new_value.data(), nullptr, 0);
					g_app->push_warning("Ammended name!", 1.f, WARNING_SUCCESS);
					break;
				default: break;
				}
			}

			m_ammend_target = nullptr;
		}
	}
}