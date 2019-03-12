#ifndef RANDOM_UTIL_H
#define RANDOM_UTIL_H
#include <random>
#include <array>
#include "..\typedefs.h"

#define RNG_USE_DOUBLE 0

namespace util {
	class random_number_generator {
	public:
	#if _WIN64
		using generator = std::mt19937_64;
	#else
		using generator = std::mt19937;
	#endif
		using device = std::random_device;
		using result_type = generator::result_type;
	#if RNG_USE_DOUBLE
		using real_type = double;
	#else
		using real_type = float;
	#endif

	public:
		// i hate singletons as much as the next person but oh well
		static random_number_generator &instance() {
			static random_number_generator rng{};
			return rng;
		}

	public:
		inline random_number_generator() {
			// seed only as many bits as there are in the generator state
			device random_device;
			std::array<int, generator::state_size> seed;
			std::generate_n(seed.data(), seed.size(), std::ref(random_device));
			std::seed_seq sequence(seed.begin(), seed.end());

			m_generator.seed(sequence);
		}

		inline random_number_generator &operator=(const random_number_generator &) = delete;
		inline random_number_generator(const random_number_generator &) = delete;

	public:
		template<typename T = int>
		inline T random_int(const T &min, const T &max) const {
			static_assert(std::is_integral_v<T>, "T must be integer type.");
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(m_generator);
		}

		template<typename T = real_type>
		inline T random_real(const T &min, const T &max) const {
			static_assert(std::is_floating_point_v<T>, "T must be real type.");
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(m_generator);
		}

	private:
		mutable generator m_generator;
	};
}

#endif