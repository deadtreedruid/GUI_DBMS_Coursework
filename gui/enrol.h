#ifndef ENROL_H
#define ENROL_H
#include "screen.h"
#include "..\student_on_course.h"
#include <map>
#include <tuple>

namespace gui {
	namespace enrol_subscreens {
		class enrol_from_app;
		class enrol_from_file;
	}

	class student_options;
	class course_options;

	class enrol_options : public screen_subscreenable {
		friend class enrol_subscreens::enrol_from_app;
		friend class enrol_subscreens::enrol_from_file;
	public:
		// add subscreens here
		enrol_options();

		void draw() override;
		void handle_event(sf::Event &evt) override;

		// composite key, low order bits are student number, high order bits are course id
		using key_type = util::uint_pair<u64, u32, u32>;
		using value_type = student_on_course;
		using iterator = std::pair<key_type, value_type>;

		std::map<key_type, value_type> m_enrolled;
	private:
		void setup();

		student_options *get_student_options_subscr();
		course_options *get_course_options_subscr();		
	};
}

#endif