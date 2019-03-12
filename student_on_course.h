#ifndef STUDENT_ON_COURSE_H
#define STUDENT_ON_COURSE_H
#include <map>
#include <tuple>

#include "util\bitwise.h"

#include "student.h"
#include "course.h"

class student_on_course {
public:
	struct assessment_progress {
		bool completed = false;
		u8 mark = std::numeric_limits<u8>::min();
		assessment_grading grade = GRADE_INVALID;
	};

	// point directly to the assessment to avoid having to assessment_map.find(id) or whatever every time we want assessment details
	template<assessment_type Type>
	using progress_map = std::map<assessment<Type> *, assessment_progress>;

	template<assessment_type Type>
	using singular_progress_mapping = std::pair<assessment<Type> *, assessment_progress>;
public:
	inline student_on_course(student &s, course &c, bool is_retake = false);

	// all these maps are WAY too expensive to be copying
	inline student_on_course(const student_on_course &) = delete;
	inline student_on_course &operator=(const student_on_course &) = delete;

	// moving is thumbs up emoji
	inline student_on_course(student_on_course &&other);
	inline student_on_course &operator=(student_on_course &&other);

	inline friend std::ostream &operator<<(std::ostream &strea, const student_on_course &soc);

	// accessors/modifiers
	inline auto &courseworks() { return m_coursework_progress; }
	inline const auto &courseworks() const { return m_coursework_progress; }
	inline auto &portfolios() { return m_portfolio_progress; }
	inline const auto &portfolios() const { return m_portfolio_progress; }
	inline auto &exams() { return m_exam_progress; }
	inline const auto &exams() const { return m_exam_progress; }
	inline auto &research_project() { return m_research_progress; }
	inline const auto &research_project() const { return m_research_progress; }

	// never allow these to be modified
	inline const auto course_ptr() const { return m_course; }
	inline const auto student_ptr() const { return m_student; }
private:
	// pointers
	course *m_course = nullptr;
	student *m_student = nullptr;
	bool m_is_retake = false;

	progress_map<ASSESSMENT_COURSEWORK> m_coursework_progress;
	progress_map<ASSESSMENT_PORTFOLIO> m_portfolio_progress;
	progress_map<ASSESSMENT_EXAM> m_exam_progress;
	singular_progress_mapping<ASSESSMENT_RESEARCHPROJ> m_research_progress;
};

student_on_course::student_on_course(student &s, course &c, bool is_retake) : m_course{ &c }, m_student{ &s }, m_is_retake{ false } {
	for (auto &coursework_assessment : c.m_coursework->assessments)
		m_coursework_progress.emplace(&coursework_assessment, assessment_progress{});

	for (auto &exam_assessment : c.m_exam->assessments)
		m_exam_progress.emplace(&exam_assessment, assessment_progress{});

	if (m_course->level() != course::LEVEL_H) {
		for (auto &portfolio_assessment : c.m_portfolio->assessments)
			m_portfolio_progress.emplace(&portfolio_assessment, assessment_progress{});
	} else {
		m_research_progress.first = c.m_research_project;
		m_research_progress.second = assessment_progress{};
	}
}

student_on_course::student_on_course(student_on_course &&other) {
	m_course = std::move(other.m_course);
	m_student = std::move(other.m_student);
	m_coursework_progress = std::move(other.m_coursework_progress);
	m_portfolio_progress = std::move(other.m_portfolio_progress);
	m_exam_progress = std::move(other.m_exam_progress);
	m_research_progress = std::move(other.m_research_progress);
}

student_on_course &student_on_course::operator =(student_on_course &&other) {
	m_course = std::move(other.m_course);
	m_student = std::move(other.m_student);
	m_coursework_progress = std::move(other.m_coursework_progress);
	m_portfolio_progress = std::move(other.m_portfolio_progress);
	m_exam_progress = std::move(other.m_exam_progress);
	m_research_progress = std::move(other.m_research_progress);
	return *this;
}

std::ostream &operator<<(std::ostream &stream, const student_on_course &soc) {
	stream << "Student: " << soc.m_student->name() << " on Course: " << soc.m_course->name();
	return stream;
}

#endif