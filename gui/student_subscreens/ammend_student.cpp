#include "ammend_student.h"
#include "..\student_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		enum ammend_criteria : size_t {
			AMMEND_ID,
			AMMEND_NAME,
			AMMEND_EMAIL,
			AMMEND_DOB,
			AMMEND_ENROLDATE
		};
		
		ammend_student::ammend_student() {
			m_name = "Ammend Student Details";
			setup_controls();
		}

		void ammend_student::draw() {
			// reset "new value" textbox
			auto category = get_element<element::dropdown>(3u);
			static auto last_category = std::numeric_limits<size_t>::max();
			if (category->get() != last_category) {
				get_element<element::textbox>(4u)->set("");
				last_category = category->get();
			}

			// only show ammend options if we have a target
			auto alter_ammend_elems = [this, &category](bool hide = true) {
				for (size_t i{ 3 }; i <= 5; ++i) {
					if (hide)
						get_element(i)->hide();
					else
						get_element(i)->show();
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

		void ammend_student::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void ammend_student::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing - (float)g_app->theme().text_size;
			add_gui_element<element::button>("Find", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_ammend_target();
			}, true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Ammend Category", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Student Number",
				"Name",
				"Email",
				"Date of Birth",
				"Enrol Date"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("New value", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing - (float)g_app->theme().text_size;
			add_gui_element<element::button, true>("Ammend", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				ammend_target();
			}, true);
		}

		void ammend_student::find_ammend_target() {
			m_ammend_target = nullptr;

			auto students = &get_parent<student_options>()->m_students;
			if (!students || (students && students->empty())) {
				g_app->push_warning("App currently holds no student data.");
				return;
			}

			auto id = get_element<element::textbox>(1u)->get().toAnsiString();
			if (id.empty()) {
				g_app->push_warning("Please enter a student number to find.", 1.5f);
				return;
			}

			auto find_id = (u32)std::stoul(id, nullptr, 0);
			auto find = students->find_if([find_id](student &s) {
				return s.student_number() == find_id;
			});

			if (find == students->end()) {
				g_app->push_warning(std::string("Couldn't find student with student number ").append(id).append("."), 1.5f);
				return;
			}

			m_ammend_target = find;
			g_app->push_warning("Found student!", 0.8f, WARNING_SUCCESS);
		}

		void ammend_student::ammend_target() {
			if (!m_ammend_target)
				return;

			auto criteria = get_element<element::dropdown>(3u)->get();
			auto new_value = get_element<element::textbox>(4u)->get().toAnsiString();
			if (new_value.empty()) {
				g_app->push_warning("Please enter a new value to ammend!");
				return;
			}

			switch (criteria) {
			case AMMEND_ID:
				if (!util::string_util::is_number_string(new_value)) {
					g_app->push_warning("New student number must be a number value.");
					return;
				}

				m_ammend_target->data.m_student_number = (u32)std::stoul(new_value, nullptr, 0);
				g_app->push_warning("Ammended student number!", 1.f, WARNING_SUCCESS);
				break;
			case AMMEND_NAME:
				m_ammend_target->data.m_name = new_value;
				g_app->push_warning("Ammended name!", 1.f, WARNING_SUCCESS);
				break;
			case AMMEND_EMAIL:
				if (!util::string_util::is_email_string(new_value)) {
					g_app->push_warning("New email must be a valid email address, check that it has and @ and a domain.", 2.5f);
					return;
				}

				m_ammend_target->data.m_email = new_value;
				g_app->push_warning("Ammended email!", 1.f, WARNING_SUCCESS);
				break;
			case AMMEND_DOB:
				if (!util::is_valid_date_string(new_value)) {
					g_app->push_warning("New date of birth must be date following XX/XX/YYYY format.", 1.5f);
					return;
				}

				m_ammend_target->data.m_dob = util::date_time(new_value);
				g_app->push_warning("Ammended date of birth!", 1.f, WARNING_SUCCESS);
				break;
			case AMMEND_ENROLDATE:
				if (!util::is_valid_date_string(new_value)) {
					g_app->push_warning("New enrol date must be date following XX/XX/YYYY format.", 1.5f);
					return;
				}

				m_ammend_target->data.m_enrol_date = util::date_time(new_value);
				g_app->push_warning("Ammended enrol date!", 1.f, WARNING_SUCCESS);
				break;
			default: break;
			}

			m_ammend_target = nullptr;
		}
	}
}