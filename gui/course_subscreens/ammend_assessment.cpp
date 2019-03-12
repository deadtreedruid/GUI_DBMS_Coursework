#include "ammend_assessment.h"
#include "..\course_options.h"
#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		enum ammend_criteria : size_t {
			AMMEND_NAME,
			AMMEND_DEADLINE
		};

		ammend_assessment::ammend_assessment() {
			m_name = "Ammend Assessment Details";
			setup_controls();
		}

		void ammend_assessment::draw() {
			// re-populate assessments dropdown
			static decltype(m_course) last_course = nullptr;
			static size_t last_assessment_type = std::numeric_limits<size_t>::max();

			auto cur_assessment_type = get_element<element::dropdown>(3u)->get();
			if (m_course != last_course || cur_assessment_type != last_assessment_type) {
				if (m_course) {
					element::names_t new_names{};
					switch (cur_assessment_type) {
					case ASSESSMENT_COURSEWORK:
						if (m_course->data.m_coursework && !m_course->data.m_coursework->assessments.empty()) {
							for (const auto &a : m_course->data.m_coursework->assessments)
								new_names.emplace_back(a.name.data());
						}
						break;
					case ASSESSMENT_EXAM:
						if (m_course->data.m_exam && !m_course->data.m_exam->assessments.empty()) {
							for (const auto &a : m_course->data.m_exam->assessments)
								new_names.emplace_back(a.name.data());
						}
						break;
					case ASSESSMENT_PORTFOLIO:
						if (m_course->data.m_portfolio && !m_course->data.m_portfolio->assessments.empty()) {
							for (const auto &a : m_course->data.m_portfolio->assessments)
								new_names.emplace_back(a.name.data());
						}
						break;
					case ASSESSMENT_RESEARCHPROJ:
						if (m_course->data.m_research_project)
							new_names.emplace_back(m_course->data.m_research_project->name.data());
						break;
					}

					get_element<element::dropdown>(4u)->alter_names(std::move(new_names));
					cur_assessment_type = last_assessment_type;
				} else {
					get_element<element::dropdown>(4u)->alter_names(element::names_t{});
				}

				last_course = m_course;
			}

			for (size_t i{ 3 }; i < m_elems.size(); ++i) {
				if (m_course)
					get_element(i)->show();
				else
					get_element(i)->hide();
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void ammend_assessment::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void ammend_assessment::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Find", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_course();
			}, true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Assessment type", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Coursework",
				"Portfolio",
				"Exam",
				"Research Project"
			}), true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Assessment", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), element::names_t{}, true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Ammend criteria", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Name",
				"Deadline"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("New value", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button, true>("Ammend", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				ammend();
			}, true);
		}

		void ammend_assessment::find_course() {
			m_course = nullptr;

			auto courses = &get_parent<course_options>()->m_courses;
			if (!courses) {
				g_app->push_warning("An unexpected error has occurred.");
				return;
			}

			if (courses->empty()) {
				g_app->push_warning("No course data is stored in the app!");
				return;
			}

			auto id = get_element<element::textbox>(1u)->get().toAnsiString();
			if (id.empty()) {
				g_app->push_warning("You must enter a course id.");
				return;
			}

			auto id32 = (u32)std::stoul(id, nullptr, 0);
			auto find = courses->find_if([id32](course &c) {
				return c.id() == id32;
			});

			if (find == courses->end()) {
				g_app->push_warning(std::string("Couldn't find course with id ").append(id));
				return;
			}

			m_course = find;
			g_app->push_warning(std::string("Found course ").append(m_course->data.name()).append("."), 1.f, WARNING_SUCCESS);
		}

		void ammend_assessment::ammend() {
			if (!m_course)
				return;

			auto assessment_dropdown = get_element<element::dropdown>(4u);
			if (!assessment_dropdown->has_names())
				return;

			auto type_dropdown = get_element<element::dropdown>(3u);
			auto type = (assessment_type)type_dropdown->get();
			auto assessment = assessment_dropdown->get();
			auto ammend_type = get_element<element::dropdown>(5u)->get();

			auto value_textbox = get_element<element::textbox>(6u);
			auto new_value = value_textbox->get().toAnsiString();
			if (new_value.empty()) {
				g_app->push_warning("You need to enter a value.");
				return;
			}

			if (ammend_type == AMMEND_DEADLINE && !util::is_valid_date_string(new_value)) {
				g_app->push_warning("New deadline must be a valid date following XX/XX/YYYY format.", 1.5f);
				return;
			}

			assessment_base *to_ammend = nullptr;
			if (type != ASSESSMENT_RESEARCHPROJ) {
				std::vector<assessment_base> *target = nullptr;
				switch (type) {
				case ASSESSMENT_COURSEWORK:
					target = (decltype(target))(&m_course->data.m_coursework->assessments);
					break;
				case ASSESSMENT_PORTFOLIO:
					target = (decltype(target))(&m_course->data.m_portfolio->assessments);
					break;
				case ASSESSMENT_EXAM:
					target = (decltype(target))(&m_course->data.m_exam->assessments);
					break;
				}

				// this shouldn't happen but just in case it does
				if (assessment >= target->size()) {
					g_app->push_warning("An unexpected error has occurred.");
					m_course = nullptr;
					return;
				} else {
					to_ammend = &target->at(assessment);		
				}
			} else {
				to_ammend = m_course->data.m_research_project;
			}

			if (!to_ammend) {
				g_app->push_warning("An unexpected error has occurred.");
				m_course = nullptr;
				return;
			}

			if (ammend_type == AMMEND_NAME)
				to_ammend->name = std::move(new_value);
			else if (ammend_type == AMMEND_DEADLINE)
				to_ammend->deadline = util::date_time(new_value);

			m_course = nullptr;
			value_textbox->set("");
			type_dropdown->set(0u);
			g_app->push_warning(std::string("Successfully ammended assessment ").append(to_ammend->name).append("!"), 0.9f, WARNING_SUCCESS);
		}
	}
}