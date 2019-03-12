#include "mark_assessment.h"

#include "..\student_options.h"
#include "..\enrol.h"
#include "..\main_menu.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\slider.h"

namespace gui {
	namespace student_subscreens {
		enum concessionals : size_t {
			CONCESSION_NONE,
			CONCESSION_NONSUBMISSION,
			CONCESSION_DEFFERED,
			CONCESSION_PLAGARISM,
			CONCESSION_LATE,
			CONCESSION_EXTENSION
		};

		mark_assessment::mark_assessment() {
			m_name = "Mark Assessment";
			setup_controls();
		}

		void mark_assessment::draw() {
			static sf::Vector2f original_positions[4];
			static bool first = true;
			if (first) {
				for (size_t i{ 1 }; i < 4; ++i)
					original_positions[i - 1] = get_element(i)->pos();
				first = !first;
			}

			if (m_data) {
				for (size_t i{ 1 }; i < 4; ++i)
					get_element(i)->reposition({ original_positions[i - 1].x - 180.f, original_positions[i - 1].y });

				// show concessional codes if mark is 0
				auto submit = get_element(8u);
				auto concessionals = get_element<element::dropdown>(7u);

				auto mark = get_element<element::textbox>(6u);
				auto mark_string = mark->get().toAnsiString();
				if (!mark_string.empty()) {
					auto mark_u8 = (u8)std::stoul(mark_string, nullptr, 0);

					static auto default_submit_pos = submit->pos();
					if (mark_u8 == std::numeric_limits<u8>::min()) {
						concessionals->show();
						submit->reposition({ default_submit_pos.x, default_submit_pos.y + 60.f });
					} else {
						concessionals->hide();
						submit->reposition(default_submit_pos);
						concessionals->set(0u);
					}
				}

				mark->show();
				submit->show();

				auto assignment_type_dropdown = get_element<element::dropdown>(4u);
				auto assignment_type = assignment_type_dropdown->get();
				auto assignments = get_element<element::dropdown>(5u);

				switch (assignment_type) {
				case ASSESSMENT_COURSEWORK:
					if (!m_data->courseworks().empty())
						populate_assessments(m_data->courseworks(), assignments);
					break;
				case ASSESSMENT_PORTFOLIO:
					if (!m_data->portfolios().empty())
						populate_assessments(m_data->portfolios(), assignments);
					break;
				case ASSESSMENT_EXAM:
					if (!m_data->exams().empty())
						populate_assessments(m_data->exams(), assignments);
					break;
				case ASSESSMENT_RESEARCHPROJ:
					assignments->alter_names({ m_data->research_project().first->name.data() });
					break;
				}

				assignment_type_dropdown->show();
				assignments->show();
			} else {
				for (size_t i{ 4 }; i < m_elems.size(); ++i)
					get_element(i)->hide();

				// get rid of all the assignment names
				get_element<element::dropdown>(5u)->alter_names({});

				for (size_t i{ 1 }; i < 4; ++i)
					get_element(i)->reposition(original_positions[i - 1]);
			}

			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		void mark_assessment::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void mark_assessment::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				this->get_parent<screen_subscreenable>()->to_root();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::textbox>("Course ID", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("Find", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_data();
			}, true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Assignment Type", sf::Vector2f(center_x - 90.f, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"Coursework",
				"Portfolio",
				"Exam",
				"Research Project"
			}), true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Assignment", sf::Vector2f(center_x - 90.f, y_offset), sf::Vector2f(180.f, height), element::names_t{}, true);


			y_offset = screen_y * 0.3f;;
			add_gui_element<element::textbox, true>("Mark", sf::Vector2f(center_x + 90.f, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += spacing;
			add_gui_element<element::dropdown, true>("Concessional Code", sf::Vector2f(center_x + 90.f, y_offset), sf::Vector2f(180.f, height), std::move(element::names_t{
				"None",
				"Non Submission",
				"Deffered",
				"Plagarism",
				"Late Submission",
				"Extension Granted"
			}), true);

			y_offset -= spacing;
			y_offset += height + 10.f;
			add_gui_element<element::button, true>("Submit", sf::Vector2f(center_x + 90.f, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				update_mark();
			}, true);
		}

		void mark_assessment::find_data() {
			m_data = nullptr;

			auto student_number = get_element<element::textbox>(1u)->get().toAnsiString();
			if (student_number.empty()) {
				g_app->push_warning("You must enter a student number.");
				return;
			}

			if (!util::string_util::is_number_string(student_number)) {
				g_app->push_warning("Student number must be a number.");
				return;
			}

			auto course_id = get_element<element::textbox>(2u)->get().toAnsiString();
			if (course_id.empty()) {
				g_app->push_warning("You must enter a course ID.");
				return;
			}

			if (!util::string_util::is_number_string(course_id)) {
				g_app->push_warning("Course ID must be a number.");
				return;
			}

			// get enrollments from the enrol options subscreen
			// god i need better ways to do this
			auto enrol_subscreen = get_parent()->get_parent<main_menu>()->get_subscreen<enrol_options>(2u);
			auto enrolments = enrol_subscreen ? &enrol_subscreen->m_enrolled : nullptr;
			if (!enrolments) {
				g_app->push_warning("An unexpected error occured.");
				return;
			}

			u32 course_32 = (u32)std::stoul(course_id, nullptr, 0);
			u32 student_32 = (u32)std::stoul(student_number, nullptr, 0);

			bool course_exists = false;
			for (auto it = enrolments->begin(); it != enrolments->end(); ++it) {
				auto &key = it->first;
				if (key.hibits() != course_32)
					continue;

				course_exists = true;
				if (key.lobits() != student_32)
					continue;

				m_data = &it->second;
			}

			if (m_data)
				g_app->push_warning("Found assessment details!", 1.f, WARNING_SUCCESS);
			else {
				if (course_exists)
					g_app->push_warning(std::string("Student ").append(student_number).append(" isn't enrolled on course ").append(course_id));
				else
					g_app->push_warning(std::string("No students are enrolled on course ").append(course_id).append(" or it doesn't exist."));
			}
		}

		template<assessment_type Type>
		void mark_assessment::populate_assessments(const student_on_course::progress_map<Type> &map, element::dropdown *gui_elem) {
			if (map.empty() || !gui_elem)
				return;

			element::names_t names{};
			for (auto it = map.cbegin(); it != map.cend(); ++it) {
				auto assessment_ptr = it->first;
				if (!assessment_ptr)
					continue;

				names.emplace_back(assessment_ptr->name.data());
			}

			gui_elem->alter_names(std::move(names));
		}

		void mark_assessment::update_mark() {
			if (!m_data)
				return;

			auto assessment_elem = get_element<element::dropdown>(5u);
			if (!assessment_elem->has_names()) {
				g_app->push_warning("Student has no assessments for this course.");
				return;
			}

			auto type = get_element<element::dropdown>(4u)->get();

			if (type == ASSESSMENT_PORTFOLIO && m_data->course_ptr()->level() == course::LEVEL_H) {
				g_app->push_warning("Level H students shouldn't have portfolios.");
				return;
			} else if (type == ASSESSMENT_RESEARCHPROJ && m_data->course_ptr()->level() != course::LEVEL_H) {
				g_app->push_warning("Only level H students should have a research project.");
				return;
			}

			student_on_course::assessment_progress *target = nullptr;
			size_t i{};
			switch (type) {
			case ASSESSMENT_COURSEWORK:
				if (assessment_elem->get() > m_data->courseworks().size()) {
					// shouldn't be able to happen in a single threaded applicaiton but hey who knows
					g_app->push_warning("Invalid assignment picked. Please try again.");
					return;
				}

				for (auto &it : m_data->courseworks()) {
					if (i == assessment_elem->get()) {
						target = &it.second;
						break;
					}
					++i;
				}
				break;
			case ASSESSMENT_PORTFOLIO:
				if (assessment_elem->get() > m_data->portfolios().size()) {
					g_app->push_warning("Invalid assignment picked. Please try again.");
					return;
				}

				for (auto &it : m_data->portfolios()) {
					if (i == assessment_elem->get()) {
						target = &it.second;
						break;
					}
					++i;
				}
				break;
			case ASSESSMENT_EXAM:
				if (assessment_elem->get() > m_data->exams().size()) {
					// shouldn't be able to happen in a single threaded applicaiton but hey who knows
					g_app->push_warning("Invalid assignment picked. Please try again.");
					return;
				}

				for (auto &it : m_data->exams()) {
					if (i == assessment_elem->get()) {
						target = &it.second;
						break;
					}
					++i;
				}
				break;
			case ASSESSMENT_RESEARCHPROJ:
				target = &m_data->research_project().second;
				break;
			default: break;
			}

			// should never happen since assignments are updated for each enrolled student as they're added
			if (!target) {
				g_app->push_warning("Student doesn't seem to have that assignment.");
				return;
			}

			auto mark = get_element<element::textbox>(6u)->get().toAnsiString();
			auto mark_u8 = (u8)std::stoul(mark, nullptr, 0);
			auto concessional = get_element<element::dropdown>(7u)->get();

			if (mark_u8 != std::numeric_limits<u8>::min()) {
				target->completed = true;
				target->mark = mark_u8;
				target->grade = mark_to_grade(mark_u8);
			} else {
				switch (concessional) {
				case CONCESSION_NONE:
					target->completed = true;
					target->grade = GRADE_UNCLASSIFIED;
					break;
				case CONCESSION_NONSUBMISSION:
					target->completed = false;
					target->grade = GRADE_NON_SUBMISSION;
					break;
				case CONCESSION_DEFFERED:
					target->completed = false;
					target->grade = GRADE_DEFFERED;
					break;
				case CONCESSION_LATE:
					target->completed = true;
					target->grade = GRADE_LATE_SUBMISSION;
					break;
				case CONCESSION_EXTENSION:
					target->completed = false;
					target->grade = GRADE_EXTENSION;
					break;
				default: break;
				}

				target->mark = mark_u8;
			}

			g_app->push_warning("Updated assessment marks!", 0.9f, WARNING_SUCCESS);
			m_data = nullptr;
		}
	}
}