#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include <SFML\Graphics.hpp>
#include <vector>
#include "..\typedefs.h"

/*
	I had free time
	Partially adapted from/inspired by javascript code by Vincent Garreau under an MIT license
*/
namespace gui {
	class particle_system {
	public:
		static constexpr u32 max_particles = 128u;
		static constexpr float max_connection_distance = 150.f;
		static constexpr int max_particle_alpha = 25;
		static constexpr int min_particle_alpha = 10;
		static constexpr float max_particle_speed = 0.15f;
		static constexpr float min_particle_speed = -max_particle_speed;
		static constexpr float particle_speedup = 0.9f;
		static constexpr float particle_radius = 2.f;
	public:
		class particle {
		public:
			inline particle() = default;
			inline particle(const sf::Vector2f &position, u8 alpha, const sf::Vector2f &velocity, particle_system *parent) :
				m_position{ position },
				m_alpha{ alpha },
				m_velocity{ velocity },
				m_parent{ parent } {

			}

			void draw() const;
			void simulate();
			void draw_connection(const particle &to) const;

			inline const auto &position() const { return m_position; }
		private:
			sf::Vector2f get_simulated_position() const;

			sf::Vector2f m_position{}, m_velocity{};
			u8 m_alpha{};
			particle_system *m_parent = nullptr;
		};

		struct connection_t {
			sf::Vector2f to;
			sf::Vector2f from;
			u8 alpha;
		};
	public:
		particle_system();
		~particle_system();

		void draw();
		void update_particle_count(size_t new_count);
	private:
		void find_all_connectors(const particle &from);
		std::vector<particle> m_particles;
		size_t m_particle_count;
	};
}

#endif