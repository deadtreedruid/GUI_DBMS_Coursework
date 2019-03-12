#ifndef COURSE_H
#define COURSE_H
#include "typedefs.h"
#include "util\datetime.h"
#include "util\fnv.h"
#include "assessment.h"
#include <algorithm>

namespace gui { 
	namespace course_subscreens { 
		class ammend_course; 
		class add_assessment;
		class ammend_assessment;
	} 
}

class student_on_course;

constexpr u32 base_course_id = 0;
class course {
	static inline u32 c_id{ base_course_id };

	friend class student_on_course;
	friend class gui::course_subscreens::ammend_course;
	friend class gui::course_subscreens::add_assessment;
	friend class gui::course_subscreens::ammend_assessment;
public:
	static constexpr auto bad_id = std::numeric_limits<u32>::max();

	enum credit_bearing : u8 {
		CREDITS_0,
		CREDITS_15,
		CREDITS_30,
		CREDITS_45,
		CREDITS_60,
		CREDITS_90,
		CREDITS_120,
		CREDITS_INVALID
	};

	enum course_level : u8 {
		LEVEL_C,
		LEVEL_I,
		LEVEL_H,
		LEVEL_M,
		LEVEL_INVALID
	};
public:
	inline course() = default;
	inline course(const std::string &name, const std::string &begin_date, const std::string &end_date, course_level lvl, credit_bearing cred);
	inline course(const std::string &name, const util::date_time &begin_date, const util::date_time &end_date, course_level lvl, credit_bearing cred);
	inline ~course();
	inline void make_assessment_sets();

	inline bool valid_course() const { return m_id != bad_id && m_credit != CREDITS_INVALID && m_level != LEVEL_INVALID; }
	inline const auto &level() const { return m_level; }
	inline const auto &name() const { return m_name; }
	inline auto name() { return m_name; }
	inline const auto &id() const { return m_id; }
	inline const auto &start_date() const { return m_begin; }
	inline const auto &end_date() const { return m_end; }
	inline auto start_date() { return m_begin; }
	inline auto end_date() { return m_end; }
	inline std::string level_as_string() const;
	inline std::string credit_as_string() const;

	static inline course from_strings(const std::string &id, const std::string &name, const std::string &begin_date, const std::string &end_date, const std::string &course_level, const std::string &credit_bearing);
private:
	inline void weight_assessments();
protected:
	u32 m_id;
	util::date_time m_begin, m_end;
	std::string m_name;
	course_level m_level;
	credit_bearing m_credit;
	
	// new and delete these to save on memory
	assessment_set<ASSESSMENT_COURSEWORK> *m_coursework = nullptr;
	assessment_set<ASSESSMENT_EXAM> *m_exam = nullptr;
	assessment_set<ASSESSMENT_PORTFOLIO> *m_portfolio = nullptr;
	assessment<ASSESSMENT_RESEARCHPROJ> *m_research_project = nullptr;
};

course::course(const std::string &name, const std::string &begin_date, const std::string &end_date, course_level lvl, credit_bearing cred) :
	m_name{ name },
	m_begin{ util::date_time(begin_date) },
	m_end{ util::date_time(end_date) },
	m_level{ lvl },
	m_credit{ cred } {
	m_id = c_id++;
	make_assessment_sets();
}

course::course(const std::string &name, const util::date_time &begin_date, const util::date_time &end_date, course_level lvl, credit_bearing cred) :
	m_name{ name },
	m_begin{ begin_date },
	m_end{ end_date },
	m_level{ lvl },
	m_credit{ cred } {
	m_id = c_id++;
	make_assessment_sets();
}

course::~course() {
	safe_delete(m_coursework);
	safe_delete(m_exam);
	safe_delete(m_portfolio);
	safe_delete(m_research_project);
}

void course::make_assessment_sets() {
	m_coursework = new assessment_set<ASSESSMENT_COURSEWORK>();
	m_exam = new assessment_set<ASSESSMENT_EXAM>();
	
	if (m_level == LEVEL_H)
		m_research_project = new assessment<ASSESSMENT_RESEARCHPROJ>();
	else
		m_portfolio = new assessment_set<ASSESSMENT_PORTFOLIO>();
	weight_assessments();
}

void course::weight_assessments() {
	switch (m_level) {
	case course_level::LEVEL_C:
		m_coursework->total_weight = u8(34);
		m_exam->total_weight = u8(33);
		m_portfolio->total_weight = u8(33);
		break;
	case course_level::LEVEL_I:
		m_coursework->total_weight = u8(50);
		m_exam->total_weight = u8(25);
		m_portfolio->total_weight = u8(25);
		break;
	case course_level::LEVEL_H:
		m_coursework->total_weight = u8(67);
		m_exam->total_weight = u8(33);
		break;
	case course_level::LEVEL_M:
		break;
	default: break;
	}
}

std::string course::level_as_string() const {
	std::string ret;
	switch (m_level) {
	case LEVEL_C:
		ret = "Level C";
		break;
	case LEVEL_I:
		ret = "Level I";
		break;
	case LEVEL_H:
		ret = "Level H";
		break;
	case LEVEL_M:
		ret = "Level M";
		break;
	case LEVEL_INVALID:
		ret = "Invalid Level";
		break;
	default: break;
	}
	return std::move(ret);
}

std::string course::credit_as_string() const {
	std::string ret;
	switch (m_credit) {
	case CREDITS_0:
		ret = "No credit bearing";
		break;
	case CREDITS_15:
		ret = "15";
		break;
	case CREDITS_30:
		ret = "30";
		break;
	case CREDITS_45:
		ret = "45";
		break;
	case CREDITS_60:
		ret = "60";
		break;
	case CREDITS_90:
		ret = "90";
		break;
	case CREDITS_120:
		ret = "120";
		break;
	case CREDITS_INVALID:
		ret = "Invalid";
		break;
	default: break;
	}
	return std::move(ret);
}

course course::from_strings(const std::string &id, const std::string &name, const std::string &begin_date, const std::string &end_date, const std::string &course_level, const std::string &credit_bearing) {
	course ret;
	ret.m_name = name;

	if (std::all_of(id.begin(), id.end(), [](char c) { return isdigit(c); }))
		ret.m_id = (u32)std::stoul(id, nullptr, 0);

	ret.m_begin = util::date_time(begin_date);
	ret.m_end = util::date_time(end_date);

	auto hashed_course_level = util::hash(course_level)();
	switch (hashed_course_level) {
	case util::hash::compile_time("Level C"):
		ret.m_level = LEVEL_C;
		break;
	case util::hash::compile_time("Level I"):
		ret.m_level = LEVEL_I;
		break;
	case util::hash::compile_time("Level H"):
		ret.m_level = LEVEL_H;
		break;
	case util::hash::compile_time("Level M"):
		ret.m_level = LEVEL_M;
		break;
	case util::hash::compile_time("Invalid Level"):
		ret.m_level = LEVEL_INVALID;
		break;
	default: break;
	}

	auto hashed_cred_bearing = util::hash(credit_bearing)();
	switch (hashed_cred_bearing) {
	case util::hash::compile_time("No credit bearing"):
		ret.m_credit = CREDITS_0;
		break;
	case util::hash::compile_time("15"):
		ret.m_credit = CREDITS_15;
		break;
	case util::hash::compile_time("30"):
		ret.m_credit = CREDITS_30;
		break;
	case util::hash::compile_time("45"):
		ret.m_credit = CREDITS_45;
		break;
	case util::hash::compile_time("60"):
		ret.m_credit = CREDITS_60;
		break;
	case util::hash::compile_time("90"):
		ret.m_credit = CREDITS_90;
		break;
	case util::hash::compile_time("120"):
		ret.m_credit = CREDITS_120;
		break;
	case util::hash::compile_time("Invalid"):
		ret.m_credit = CREDITS_INVALID;
		break;
	default: break;
	}

	return std::move(ret);
}

#endif