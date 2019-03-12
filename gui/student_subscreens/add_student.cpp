#include "add_student.h"
#include "..\student_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		add_student::add_student() {
			m_name = "Add Student";
			setup_controls();
		}

		void add_student::draw() {
			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void add_student::handle_event(sf::Event &evt) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void add_student::setup_controls() {
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
			add_gui_element<element::textbox>("Email", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Date of Birth", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Enrol Date", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Auto-fill Enrol Date", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				this->get_element<element::textbox>(4u)->set(util::date_time::now().to_string());
			}, true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Submit", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				submit();
			}, true);
		}

		void add_student::submit() {
			auto students = &get_parent<student_options>()->m_students;
			if (!students)
				return;

			auto name = get_element<element::textbox>(1u)->get().toAnsiString();
			if (name.empty()) {
				g_app->push_warning("You must enter a name!", 1.5f);
				return;
			}

			auto email = get_element<element::textbox>(2u)->get().toAnsiString();
			if (email.empty() || !util::string_util::is_email_string(email)) {
				g_app->push_warning("Email isn't valid, make sure it contains an @ and a domain at the end.", 2.f);
				return;
			}

			auto dob = get_element<element::textbox>(3u)->get().toAnsiString();
			if (dob.empty() || !util::is_valid_date_string(dob)) {
				g_app->push_warning("Date of Birth must be valid date following XX/XX/YYYY format!", 2.f);
				return;
			}

			auto enrol_date = get_element<element::textbox>(4u)->get().toAnsiString();
			if (enrol_date.empty() || !util::is_valid_date_string(enrol_date)) {
				g_app->push_warning("Enrol Date must be valid date following XX/XX/YYYY format!", 2.f);
				return;
			}

			students->emplace_back(
				name,
				email,
				dob,
				enrol_date
			);

			g_app->push_warning(std::string("Added student ").append(name).append(" with student number ").append(std::to_string(students->back()->data.student_number())), 1.25f, WARNING_SUCCESS);
			reset();
		}

		void add_student::reset() {
			for (size_t i{ 1 }; i <= 4u; ++i)
				((element::textbox *)get_element(i))->set("");
		}
	}
}