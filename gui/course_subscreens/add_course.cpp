#include "add_course.h"
#include "..\course_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		add_course::add_course() {
			m_name = "Add Course";
			setup_controls();
		}

		void add_course::draw() {
			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void add_course::handle_event(sf::Event &evt) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void add_course::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Name", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);
			
			y_offset += spacing;
			add_gui_element<element::textbox>("Start Date", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::textbox>("End Date", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::dropdown>("Level", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"C",
				"I",
				"H"
			}),
			true);

			y_offset += spacing;
			add_gui_element<element::dropdown>("Credit Bearing", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"0",
				"15",
				"30",
				"45",
				"60",
				"90",
				"120"
			}),
			true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Submit", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				submit();
			}, true);
		}

		void add_course::submit() {
			auto name = get_element<element::textbox>(1u)->get().toAnsiString();
			if (name.empty()) {
				g_app->push_warning("You must enter a name!", 1.5f);
				return;
			}

			auto start_date = get_element<element::textbox>(2u)->get().toAnsiString();
			if (start_date.empty() || !util::is_valid_date_string(start_date)) {
				g_app->push_warning("Start Date must be valid date following XX/XX/YYYY format!", 1.2f);
				return;
			}

			auto end_date = get_element<element::textbox>(3u)->get().toAnsiString();
			if (end_date.empty() || !util::is_valid_date_string(end_date)) {
				g_app->push_warning("End Date must be valid date following XX/XX/YYYY format!", 1.2f);
				return;
			}

			auto start_dt = util::date_time(start_date);
			auto end_dt = util::date_time(end_date);
			if (start_dt >= end_dt) {
				g_app->push_warning("End date must be after the start date!", 1.5f);
				return;
			}

			auto level = (course::course_level)get_element<element::dropdown>(4u)->get();
			auto credit_bearing = (course::credit_bearing)get_element<element::dropdown>(5u)->get();

			auto courses = &get_parent<course_options>()->m_courses;
			if (!courses) {
			#ifdef _DEBUG
				printf("load_course_from_file::load() - parent course details are nullptr");
			#endif
				return;
			}

			courses->emplace_back(
				name,
				start_dt,
				end_dt,
				level,
				credit_bearing
			);
			
			auto msg = std::string("Added course ").append(courses->back()->data.name()).append(" with course id ").append(std::to_string(courses->back()->data.id()));
			g_app->push_warning(msg, 1.5f, WARNING_SUCCESS);
		#ifdef _DEBUG
			printf("%s\n", msg.data());
		#endif
		}
	}
}