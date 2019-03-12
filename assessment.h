#ifndef ASSESSMENT_H
#define ASSESSMENT_H
#include <type_traits>
#include "typedefs.h"
#include "util\datetime.h"
#include "grading.h"

enum assessment_type : u8 {
	ASSESSMENT_COURSEWORK,
	ASSESSMENT_PORTFOLIO,
	ASSESSMENT_EXAM,
	ASSESSMENT_RESEARCHPROJ
};


struct assessment_base {
	inline assessment_base() = default;
	std::string name = "";
	util::date_time deadline{};
};

// lets us enforce assessment typing
template<assessment_type Type>
struct assessment : public assessment_base { };

template<assessment_type Type>
struct assessment_set {
	inline assessment_set() = default;
	std::vector<assessment<Type>> assessments{};
	u8 total_weight{};
};

#endif