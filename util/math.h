#ifndef MATH_H
#define MATH_H
#if _WIN32
	#include <intrin.h>
#endif // _WIN32
#include "../typedefs.h"

namespace util {
	// Optimised clamp function since std::clamp is really slow and bad
	template<typename T>
	inline void Clamp(T &value, const T &min, const T &max) {
#if _WIN32
		value = (T)_mm_cvtss_f32(_mm_min_ss(_mm_max_ss(_mm_set_ss((float)value), _mm_set_ss((float)min)), _mm_set_ss((float)max)));
#else
		if (value > max)
			value = max;
		else if (value < min)
			value = min;
#endif // _WIN32
	}
} // namespace util

#endif MATH_H // STRING_UTIL_H