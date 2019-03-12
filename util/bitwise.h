#ifndef BITWISE_UTIL_H
#define BITWISE_UTIL_H
#include "..\typedefs.h"
#include <limits>
#include <type_traits>

namespace util {
	// makes an unsigned integer of type T out of any two unsigned integers
	template<typename T, typename Lo, typename Hi>
	class uint_pair {
		using lo_limits = std::numeric_limits<Lo>;
		using hi_limits = std::numeric_limits<Hi>;
	public:
		constexpr explicit uint_pair(Lo lobits, Hi hibits) {
			static_assert(std::is_unsigned_v<T> && std::is_unsigned_v<Lo> && std::is_unsigned_v<Hi>, "uint_pair need unsigned types");
			static_assert((sizeof(Lo) + sizeof(Hi)) == sizeof(T), "Lo and Hi must make uint with size of type T");

			constexpr auto hi_bitcount = sizeof(Hi) * 8;
			m_value = (((T)hibits << hi_bitcount) | (T)lobits);
		}

		constexpr Hi hibits() const {
			constexpr auto hi_bitcount = sizeof(Hi) * 8;
			return m_value >> hi_bitcount;
		}

		constexpr Lo lobits() const {
			constexpr auto mask = lo_limits::max();
			return mask & m_value;
		}

		// treat this type as T
		constexpr operator T() { return m_value; }
		constexpr operator const T() const { return m_value; }
		constexpr T operator()() { return m_value; }
		constexpr const T &operator()() const { return m_value; }
		constexpr auto value() { return m_value; }
		constexpr const auto &value() const { return m_value; }
	private:
		T m_value{};
	};
}

#endif