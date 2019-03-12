#include "view_transcript.h"

#include "..\student_options.h"
#include "..\main_menu.h"
#include "..\enrol.h"

#include "..\..\student.h"
#include "..\..\assessment.h"
#include "..\..\grading.h"

#include "..\elements\button.h"
#include "..\elements\textbox.h"
#include "..\elements\dropdown.h"
#include "..\elements\slider.h"

/*
	Tried to make this functional but there's just too much information to show for it all to fit on screen
	and I didn't have time to write scrolling pages
*/
namespace gui {
	namespace student_subscreens {
		view_transcript::view_transcript() {
			m_name = "View Transcript";
			setup_controls();
		}

		void view_transcript::draw() {
			if (!m_data.empty()) {
				m_next_free_elem = 3u;

				get_element(1)->hide();
				get_element(2)->hide();

				auto y_offset = (float)g_app->window()->getSize().y * 0.18f;
				constexpr auto x_pos = 30.f;

				static std::string name_info;
				bool first_frame_for_this_transcript = false;

				// student data will be the same on all entries so grab back() since its the least expensive accessor
				auto &back = m_data.back();
				
				if (back->student_ptr() != m_last) {
					name_info = std::string(back->student_ptr()->name()).append(" - ").append(std::to_string(back->student_ptr()->student_number()));
					m_last = back->student_ptr();
					first_frame_for_this_transcript = true;
				}

				// student name - student number
				g_app->draw_string(
					name_info.data(), 
					g_app->theme().text_size + 5, 
					sf::Vector2f(x_pos, y_offset),
					g_app->theme().text_color
				);

				y_offset += (float)g_app->theme().text_size + 5.f + 10.f;
				for (auto &course : m_data) {
					g_app->draw_string(course->course_ptr()->name().data(), g_app->theme().text_size, { x_pos, y_offset }, g_app->theme().text_color);
					y_offset += (float)g_app->theme().text_size + 10.f;

					// assessment details
					if (!course->courseworks().empty())
						draw_assessment_set(course->courseworks(), y_offset, x_pos, first_frame_for_this_transcript);

					if (!course->portfolios().empty())
						draw_assessment_set(course->courseworks(), y_offset, x_pos, first_frame_for_this_transcript);

					if (!course->exams().empty())
						draw_assessment_set(course->courseworks(), y_offset, x_pos, first_frame_for_this_transcript);
				}		
			} else {
				get_element(1)->show();
				get_element(2)->show();

				// delete any new elements we made for the transcript
				if (m_elems.size() > 3) {
					for (size_t i{ 3 }; i < m_elems.size(); ++i)
						delete m_elems[i];

					m_elems.erase(m_elems.begin() + 3, m_elems.end());
				}

				m_last = nullptr;
			}
			
			for (auto elem : m_elems) {
				if (!m_focus || (m_focus && elem != m_focus))
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		}

		template<assessment_type Type>
		void view_transcript::draw_assessment_set(student_on_course::progress_map<Type> &map, float &y_offset, const float x_pos, bool setup_frame) {
			if (map.empty())
				return;

			auto text_add = (float)g_app->theme().text_size + 10.f;

			switch (Type) {
			case ASSESSMENT_COURSEWORK:
				g_app->draw_string("Courseworks", g_app->theme().text_size, { x_pos + 30.f, y_offset }, g_app->theme().text_color);
				y_offset += text_add;
				break;
			case ASSESSMENT_PORTFOLIO:
				g_app->draw_string("Portfolios", g_app->theme().text_size, { x_pos + 30.f, y_offset }, g_app->theme().text_color);
				y_offset += text_add;
				break;
			case ASSESSMENT_EXAM:
				g_app->draw_string("Exams", g_app->theme().text_size, { x_pos + 30.f, y_offset }, g_app->theme().text_color);
				y_offset += text_add;
				break;
			default: break;
			}

			auto now = util::date_time::now();

			u64 mark_total{};
			size_t assessment_count{};

			auto x_offset = x_pos + 50.f;
			for (auto &[assessment, progress] : map) {
				g_app->draw_string(assessment->name.data(), g_app->theme().text_size, { x_offset, y_offset }, g_app->theme().text_color);
				auto size = g_app->string_bounds(assessment->name, g_app->theme().text_size);

				if (progress.completed) {
					g_app->draw_string("COMPLETE", g_app->theme().text_size, { x_offset + size.x + 10.f, y_offset }, g_app->theme().warning_success);
					y_offset += text_add;

					g_app->draw_string(std::string("Grade: ").append(grade_to_string(progress.grade)).data(), g_app->theme().text_size, { x_offset + 30.f, y_offset }, g_app->theme().text_color);
					y_offset += text_add;

					g_app->draw_string(std::string("Mark: ").append(std::to_string(progress.mark)).data(), g_app->theme().text_size, { x_offset + 30.f, y_offset }, g_app->theme().text_color);
					//y_offset += text_add;

					if (progress.grade != GRADE_DEFFERED && progress.grade != GRADE_EXTENSION) {
						mark_total += (u64)progress.mark;
						++assessment_count;
					}
				} else {
					if (now > assessment->deadline && progress.grade != GRADE_EXTENSION) {
						g_app->draw_string("LATE", g_app->theme().text_size, { x_offset + size.x + 10.f, y_offset }, g_app->theme().warning_fail);
						y_offset += text_add;

						if (progress.grade != GRADE_LATE_SUBMISSION) {
							progress.grade = GRADE_LATE_SUBMISSION;
							progress.mark = std::numeric_limits<u8>::min();
							progress.completed = true;
						}
					} else {
						g_app->draw_string(
							progress.grade == GRADE_EXTENSION ? "Consult tutor for extension date" : std::string("Due ").append(assessment->deadline.to_string(true)).data(), 
							g_app->theme().text_size, { x_offset + size.x + 10.f, y_offset }, 
							g_app->theme().warning_success
						);
						y_offset += text_add;
					}
				}
			}

			y_offset += 60.f;

			auto average_mark = (float)mark_total / (float)assessment_count;
			average_mark = std::min(average_mark, 100.f);
			auto avg_u8 = (u8)std::round(average_mark);

			if (setup_frame)
				add_gui_element<element::slider<u8>>("Progress", sf::Vector2f(x_offset, y_offset), sf::Vector2f((float)g_app->window()->getSize().x * 0.5f, 10.f), 0, 100, false, 0, false);

			auto bar_index = next_free_elem();
			if (bar_index < m_elems.size()) {
				auto bar = get_element<element::slider<u8>>(bar_index);
				bar->set(avg_u8);
			}

			y_offset += 60.f;
		}

		void view_transcript::handle_event(sf::Event &evt) {
			if (m_focus) {
				m_focus->handle_event(evt);
			} else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		}

		void view_transcript::setup_controls() {
			constexpr float height = 30.f;
			constexpr float spacing = height + 30.f;

			const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
			const auto screen_y = (float)g_app->window()->getSize().y;

			add_gui_element<element::button>("Back", sf::Vector2f(30.f, screen_y - 50.f), sf::Vector2f(100.f, height), [this](element::button *btn) {
				if (m_data.empty())
					this->get_parent<screen_subscreenable>()->to_root();
				else
					reset();
			});

			auto y_offset = screen_y * 0.3f;
			add_gui_element<element::textbox>("Student Number", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), true);

			y_offset += height + 10.f;
			add_gui_element<element::button>("View Transcript", sf::Vector2f(center_x, y_offset), sf::Vector2f(180.f, height), [this](element::button *) {
				find_student();
			}, true);
		}

		void view_transcript::reset() {
			m_data.clear();
			auto stnum = get_element<element::textbox>(1u);
			stnum->set("");
			stnum->show();
			get_element(2u)->show();
			m_last = nullptr;
		}

		void view_transcript::find_student() {
			if (!m_data.empty())
				m_data.clear();

			auto student_number = get_element<element::textbox>(1u)->get().toAnsiString();
			if (student_number.empty()) {
				g_app->push_warning("Please enter a student number");
				return;
			}

			if (!util::string_util::is_number_string(student_number)) {
				g_app->push_warning("Student number must be a number.");
				return;
			}

			auto students = &get_parent<student_options>()->m_students;
			auto target = (u32)std::stoul(student_number, nullptr, 0);

			auto &enrollments = get_parent()->get_parent<main_menu>()->get_subscreen<enrol_options>(2u)->m_enrolled;
			bool student_exists = false;
			for (auto &[k, v] : enrollments) {
				if (k.lobits() != target)
					continue;

				m_data.emplace_back(&v);
			}

			if (m_data.empty()) {
				auto students = &get_parent<student_options>()->m_students;
				auto find = students->find_if([target](student &s) {
					return s.student_number() == target;
				});

				if (find == students->end())
					g_app->push_warning("Student doesn't exist in the app!");
				else
					g_app->push_warning("Student found but they aren't enrolled on any courses.");
			} else {
				g_app->push_warning("Produced transcript.", 1.f, WARNING_SUCCESS);
			}
		}
	}
}