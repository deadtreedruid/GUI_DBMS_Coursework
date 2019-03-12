#ifndef FNV_H
#define FNV_H
#include "..\typedefs.h"
#include <cstring>
#include <string>

/*
	32 and 64 bit FNV-1a string hash implementation
	Based on work by Stephan Brumme under a zlib license
*/

#ifdef __cpp_lib_string_view
#include <string_view>
#endif // __cpp_lib_string_view

#define MAKE_FNV_CONSTANTS(Type, Prime, OffsetBasis) \
template<> \
struct fnv_constants<sizeof(Type) * 8> { \
	static constexpr Type prime = Prime; \
	static constexpr Type offset_basis = OffsetBasis; \
}; \


// integral constant overflow
#pragma warning(disable:4307)

namespace util {
	namespace detail {
		template<size_t Bits>
		struct fnv_constants { };

		MAKE_FNV_CONSTANTS(u32, 16777619, 0x811c9dc5);
		MAKE_FNV_CONSTANTS(u64, 1099511628211, 0xcbf29ce484222325);
	}

	template<size_t Bits>
	class fnv {
	public:
		using hash_t = uintptr_t;
		using constants = detail::fnv_constants<Bits>;

		static inline hash_t run_time(const char *str) {
			if (!str || str[0] == '\0')
				return {};

			auto len = strlen(str);
			auto hash = constants::offset_basis;
			for (size_t i{}; i < len; ++i) {
				hash ^= *str++;
				hash *= constants::prime;
			}

			return hash;
		}

		static constexpr hash_t compile_time(const char *str, size_t i = constants::offset_basis) {
			return (str[0] == '\0') ? i : compile_time(&str[1], (i ^ hash_t(str[0])) * constants::prime);
		}

		inline fnv(const char *str) noexcept : m_value(run_time(str)) { }
		inline fnv(const std::string &str) noexcept : m_value(run_time(str.data())) { }

	#ifdef __cpp_lib_string_view
		inline fnv(std::string_view sv) noexcept : m_value(run_time(sv.data())) { }
	#endif // __cpp_lib_string_view

		inline operator hash_t() noexcept { return m_value; }
		inline operator const hash_t() const noexcept { return m_value; }
		inline auto operator()() noexcept { return m_value; }
		inline const auto operator()() const noexcept { return m_value; }
	private:
		hash_t m_value{};
	};

	using hash = fnv<sizeof(void *) * 8>;
} // namespace util

#endif // FNV_H