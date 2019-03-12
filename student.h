#ifndef STUDENT_H
#define STUDENT_H
#include <string>
#include <iostream>

#include "typedefs.h"
#include "util\datetime.h"

constexpr u32 base_student_number = 0;

namespace gui { namespace student_subscreens { class ammend_student; } }

class student_on_course;

class student {
	static inline u32 s_id{ base_student_number };

	friend class gui::student_subscreens::ammend_student;
	friend class student_on_course;
public:
	static constexpr u32 bad_id = std::numeric_limits<u32>::max();
public:
	inline student() = default;
	inline student(const std::string &name, const std::string &email, const std::string &dob, const std::string &enrol_date, const std::string &enrol_time = "");
	inline student(const std::string &name, const std::string &email, const util::date_time &dob, const util::date_time &enrol_date);
	inline student(student &&);
	inline student &operator=(student &&);
	inline student(const student &);
	inline student &operator=(const student &);

	inline friend std::ostream &operator<<(std::ostream &out, const student &data);
	inline friend std::istream &operator>>(std::istream &in, student &target);

	static inline student from_strings(const std::string &id, const std::string &name, const std::string &email, const std::string &dob, const std::string &enrol_date);
	inline bool is_valid_student() const { return m_student_number != bad_id; }

	inline const auto &student_number() const { return m_student_number; }
	inline const auto &name() const { return m_name; }
	inline const auto &email() const { return m_email; }
	inline auto email() { return m_email; }
	inline const auto &date_of_birth() const { return m_dob; }
	inline auto date_of_birth() { return m_dob; }
	inline const auto &enrollment_date() const { return m_enrol_date; }
	inline auto enrollment_date() { return m_enrol_date; }
private:
	u32 m_student_number;
	std::string m_name, m_email;
	util::date_time m_dob, m_enrol_date;
};

student::student(const std::string &name, const std::string &email, const std::string &dob, const std::string &enrol, const std::string &enrol_time) : m_name{ name }, m_email{ email } {
	m_dob = util::date_time(dob);

	if (enrol_time.empty())
		m_enrol_date = util::date_time(enrol);
	else
		m_enrol_date = util::date_time(enrol, enrol_time);

	m_student_number = s_id++;
}

student::student(const std::string &name, const std::string &email, const util::date_time &dob, const util::date_time &enrol_date) : m_name{ name }, m_email{ email }, m_dob{ dob }, m_enrol_date{ enrol_date }, m_student_number{ s_id++ } { }

student::student(student &&other) {
	m_student_number = std::move(other.m_student_number);
	m_name = std::move(other.m_name);
	m_email = std::move(other.m_email);
	m_dob = std::move(other.m_dob);
	m_enrol_date = std::move(other.m_dob);
}

student &student::operator=(student &&other) {
	m_student_number = std::move(other.m_student_number);
	m_name = std::move(other.m_name);
	m_email = std::move(other.m_email);
	m_dob = std::move(other.m_dob);
	m_enrol_date = std::move(other.m_dob);
	return *this;
}

std::ostream &operator<<(std::ostream &out, const student &data) {
	out << "Student: " << data.m_student_number << std::endl;
	out << "\tName: " << data.m_name << "\n\tDate of Birth: " << data.m_dob << "\n\tEnrollment Date: " << data.m_enrol_date << "\n\tEmail: " << data.m_email;

	return out;
}

std::istream &operator>>(std::istream &in, student &target) {
	return in;
}

student::student(const student &s) {
	memcpy((void *)this, (void *)std::addressof(s), sizeof(student));
}

student &student::operator=(const student &s) {
	memcpy((void *)this, (void *)std::addressof(s), sizeof(student));
	return *this;
}

student student::from_strings(const std::string &id, const std::string &name, const std::string &email, const std::string &dob, const std::string &enrol_date) {
	student s;
	if (!util::string_util::is_number_string(id))
		s.m_student_number = bad_id;
	else
		s.m_student_number = (u32)std::stoul(id, nullptr, 0);

	s.m_name = name;

	if (util::string_util::is_email_string(email))
		s.m_email = email;
	else
		s.m_student_number = bad_id;

	if (util::is_valid_date_string(dob))
		s.m_dob = util::date_time(dob);
	else
		s.m_student_number = bad_id;

	if (util::is_valid_date_string(enrol_date))
		s.m_enrol_date = util::date_time(enrol_date);
	else
		s.m_student_number = bad_id;

	return std::move(s);
}

#endif // STUDENT_H