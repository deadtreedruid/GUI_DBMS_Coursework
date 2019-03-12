#include "add_assessment.h"

#include "..\course_options.h"
#include "..\enrol.h"
#include "..\main_menu.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

namespace gui {
	namespace course_subscreens {
		add_assessment::add_assessment() {
			m_name = "Add Assessment";
			setup_controls();
		}

		void add_assessment::draw() {
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

		void add_assessment::handle_event(sf::Event &evt) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void add_assessment::setup_controls() {
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
			add_gui_element<element::button>("Find", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				find_course();
			}, true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Assessment Name", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Assessment Type", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Coursework",
				"Portfolio",
				"Examination",
				"Research Project"
			}), true);

			y_offset += spacing;
			add_gui_element<element::textbox, true>("Due Date", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button, true>("Submit", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *btn) {
				add();
			}, true);
		}

		void add_assessment::find_course() {
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

		void add_assessment::add() {
			if (!m_course)
				return;

			auto name = get_element<element::textbox>(3u)->get().toAnsiString();
			if (name.empty()) {
				g_app->push_warning("Assessment needs a name.");
				return;
			}

			auto type = get_element<element::dropdown>(4u)->get();
			if (type == ASSESSMENT_RESEARCHPROJ && m_course->data.level() != course::course_level::LEVEL_H) {
				g_app->push_warning("Cannot assign research project to course that isn't level H.", 0.9f);
				return;
			} else if (type == ASSESSMENT_PORTFOLIO && m_course->data.level() == course::course_level::LEVEL_H) {
				g_app->push_warning("Cannot assign portfolio assessment(s) to a level H course.", 0.9f);
			}

			auto due_date = get_element<element::textbox>(5u)->get().toAnsiString();
			if (due_date.empty()) {
				g_app->push_warning("Assessment needs a due date.");
				return;
			}

			if (!util::is_valid_date_string(due_date)) {
				g_app->push_warning("Due date must be valid string following XX/XX/YYYY format.", 1.2f);
				return;
			}

			auto enrol_subscreen = get_parent<course_options>()->get_parent<main_menu>()->get_subscreen<enrol_options>(2);
			auto enrolments = enrol_subscreen ? &enrol_subscreen->m_enrolled : nullptr;
			if (!enrolments) {
				g_app->push_warning("An unexpected error occured.");
				return;
			}

			// no way to do this without else ifs sorry
			if (type == ASSESSMENT_COURSEWORK) {
				assessment<ASSESSMENT_COURSEWORK> a;
				a.deadline = util::date_time(due_date);
				a.name = name;

				// NOTE: don't use emplace back here or weird memory stuff happens and it crashes
				auto &assessments = m_course->data.m_coursework->assessments;
				assessments.push_back(a);

				// we have to give this assignment to all the students that were enrolled on the course before having this added
				if (!enrolments->empty()) {
					for (auto it = enrolments->begin(); it != enrolments->end(); ++it) {
						if (it->first.hibits() != m_course->data.id())
							continue;

						it->second.courseworks().emplace(&assessments.back(), student_on_course::assessment_progress{});
					}
				}
			} else if (type == ASSESSMENT_PORTFOLIO) {
				assessment<ASSESSMENT_PORTFOLIO> a;
				a.deadline = util::date_time(due_date);
				a.name = name;

				auto &assessments = m_course->data.m_portfolio->assessments;
				assessments.push_back(a);

				if (!enrolments->empty()) {
					for (auto it = enrolments->begin(); it != enrolments->end(); ++it) {
						if (it->first.hibits() != m_course->data.id())
							continue;

						it->second.portfolios().emplace(&assessments.back(), student_on_course::assessment_progress{});
					}
				}
			} else if (type == ASSESSMENT_EXAM) {
				assessment<ASSESSMENT_EXAM> a;
				a.deadline = util::date_time(due_date);
				a.name = name;

				auto &assessments = m_course->data.m_exam->assessments;
				assessments.push_back(a);

				if (!enrolments->empty()) {
					for (auto it = enrolments->begin(); it != enrolments->end(); ++it) {
						if (it->first.hibits() != m_course->data.id())
							continue;

						it->second.exams().emplace(&assessments.back(), student_on_course::assessment_progress{});
					}
				}
			} else {
				m_course->data.m_research_project->deadline = util::date_time(due_date);
				m_course->data.m_research_project->name = name;

				if (!enrolments->empty()) {
					for (auto it = enrolments->begin(); it != enrolments->end(); ++it) {
						if (it->first.hibits() != m_course->data.id())
							continue;

						it->second.research_project().first = m_course->data.m_research_project;
						it->second.research_project().second = student_on_course::assessment_progress{};
					}
				}
			}

			g_app->push_warning("Added assessment!", 0.5f, WARNING_SUCCESS);
			m_course = nullptr;
		}
	}
}