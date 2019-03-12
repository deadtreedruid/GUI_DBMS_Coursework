#ifndef GRADING_H
#define GRADING_H
#include "typedefs.h"
#include <string>

#define GRADE_RANGE(Grade, Min, Max) \
template<> \
struct grade_range<Grade> { \
	static constexpr u8 min = Min; \
	static constexpr u8 max = Max; \
	static constexpr decltype(Grade) grade = Grade; \
}; \

enum assessment_grading : u8 {
	// unexplained non-submission of assessment
	GRADE_NON_SUBMISSION,

	// deffered due to valid mitigating circumstances
	GRADE_DEFFERED,

	// plagarism or other academic misconduct - reduce mark to 0
	GRADE_PLAGARISM,

	// late - reduce mark to 0
	GRADE_LATE_SUBMISSION,

	// extension granted
	GRADE_EXTENSION,

	// do not allow re-sit even if the student has passed the block overall
	GRADE_UNCLASSIFIED,

	// same as above
	GRADE_WEAK_FAIL,

	// comprehensive fail - same as above
	GRADE_F_MINUS,

	// clear fail - allowed a re-sit
	GRADE_F,

	// marginal fail - allowed a re-sit
	GRADE_F_PLUS,

	// low 3rd class honours
	GRADE_E,

	// high 3rd class honours
	GRADE_D,

	// 2ii/3rd class borderline
	GRADE_C_MINUS,

	// 2ii honours
	GRADE_C,

	// high 2ii honours
	GRADE_C_PLUS,

	// 2ii/2i borderline
	GRADE_B_MINUS,

	// 2i honours
	GRADE_B,

	// high 2i honours
	GRADE_B_PLUS,

	// 2i/1st class borderline
	GRADE_A_MINUS,

	// low 1st class
	GRADE_A,

	// mid 1st class
	GRADE_A_PLUS,

	// high 1st class
	GRADE_A_PLUS_PLUS,
	GRADE_INVALID
};

template<assessment_grading>
struct grade_range { };

GRADE_RANGE(GRADE_UNCLASSIFIED, 0, 2);
GRADE_RANGE(GRADE_WEAK_FAIL, 3, 12);
GRADE_RANGE(GRADE_F_MINUS, 13, 22);
GRADE_RANGE(GRADE_F, 23, 32);
GRADE_RANGE(GRADE_F_PLUS, 33, 38);
GRADE_RANGE(GRADE_E, 42, 47);
GRADE_RANGE(GRADE_D, 48, 49);
GRADE_RANGE(GRADE_C_MINUS, 50, 53);
GRADE_RANGE(GRADE_C, 54, 57);
GRADE_RANGE(GRADE_C_PLUS, 58, 59);
GRADE_RANGE(GRADE_B_MINUS, 60, 63);
GRADE_RANGE(GRADE_B, 64, 67);
GRADE_RANGE(GRADE_B_PLUS, 68, 69);
GRADE_RANGE(GRADE_A_MINUS, 70, 75);
GRADE_RANGE(GRADE_A, 76, 83);
GRADE_RANGE(GRADE_A_PLUS, 84, 91);
GRADE_RANGE(GRADE_A_PLUS_PLUS, 92, 100);

namespace detail {
	template<assessment_grading G>
	constexpr bool is_in_range(u8 mark) {
		using range = grade_range<G>;
		return mark >= range::min && mark <= range::max;
	}
}

#define RETURN_IF_IN_RANGE_FIRST(Grade, Mark) \
if (detail::is_in_range<Grade>(mark)) \
	return Grade; \

#define RETURN_IF_IN_RANGE(Grade, Mark) \
else if (detail::is_in_range<Grade>(mark)) \
	return Grade; \

inline assessment_grading mark_to_grade(u8 mark) {
	mark = std::min(mark, u8(100));

	// can't think of a way to do this recursively and im too tired to spend more time on it
	RETURN_IF_IN_RANGE_FIRST(GRADE_UNCLASSIFIED, mark)
		RETURN_IF_IN_RANGE(GRADE_WEAK_FAIL, mark)
		RETURN_IF_IN_RANGE(GRADE_F_MINUS, mark)
		RETURN_IF_IN_RANGE(GRADE_F, mark)
		RETURN_IF_IN_RANGE(GRADE_F_PLUS, mark)
		RETURN_IF_IN_RANGE(GRADE_E, mark)
		RETURN_IF_IN_RANGE(GRADE_D, mark)
		RETURN_IF_IN_RANGE(GRADE_C_MINUS, mark)
		RETURN_IF_IN_RANGE(GRADE_C, mark)
		RETURN_IF_IN_RANGE(GRADE_C_PLUS, mark)
		RETURN_IF_IN_RANGE(GRADE_B_MINUS, mark)
		RETURN_IF_IN_RANGE(GRADE_B, mark)
		RETURN_IF_IN_RANGE(GRADE_B_PLUS, mark)
		RETURN_IF_IN_RANGE(GRADE_A_MINUS, mark)
		RETURN_IF_IN_RANGE(GRADE_A, mark)
		RETURN_IF_IN_RANGE(GRADE_A_PLUS, mark)
		RETURN_IF_IN_RANGE(GRADE_A_PLUS_PLUS, mark)
else return GRADE_INVALID;
}

#define GRADE2STR(Grade, Str) \
case Grade: \
	ret = Str; \
	break; \

inline std::string grade_to_string(assessment_grading g) {
	std::string ret;

	switch (g) {
		GRADE2STR(GRADE_A_PLUS_PLUS, "A++")
		GRADE2STR(GRADE_A_PLUS, "A+")
		GRADE2STR(GRADE_A, "A")
		GRADE2STR(GRADE_A_MINUS, "A-")
		GRADE2STR(GRADE_B_PLUS, "B+")
		GRADE2STR(GRADE_B, "B")
		GRADE2STR(GRADE_B_MINUS, "B-")
		GRADE2STR(GRADE_C_PLUS, "C+")
		GRADE2STR(GRADE_C, "C")
		GRADE2STR(GRADE_C_MINUS, "C-")
		GRADE2STR(GRADE_D, "D")
		GRADE2STR(GRADE_E, "E")
		GRADE2STR(GRADE_F_PLUS, "F+")
		GRADE2STR(GRADE_F, "F")
		GRADE2STR(GRADE_F_MINUS, "F-")
		GRADE2STR(GRADE_WEAK_FAIL, "WF")
		GRADE2STR(GRADE_UNCLASSIFIED, "U")
		GRADE2STR(GRADE_EXTENSION, "0EX")
		GRADE2STR(GRADE_LATE_SUBMISSION, "0L")
		GRADE2STR(GRADE_PLAGARISM, "0PL")
		GRADE2STR(GRADE_DEFFERED, "DEFFERED")
		GRADE2STR(GRADE_NON_SUBMISSION, "0NS")
	default: break;
	}

	return std::move(ret);
}

#endif