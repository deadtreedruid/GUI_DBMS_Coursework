#include "particle_system.h"
#include "gui.h"

#include "..\util\random.h"

namespace gui {
	particle_system::particle_system() {
		m_particle_count = max_particles;
		m_particles.reserve(m_particle_count);

		for (size_t i{}; i < m_particle_count; ++i) {
			m_particles.emplace_back(
				// position
				sf::Vector2f(
					util::random_number_generator::instance().random_real<float>(0.f, (float)g_app->window()->getSize().x),
					util::random_number_generator::instance().random_real<float>(0.f, (float)g_app->window()->getSize().y)
				),
				// alpha
				(u8)util::random_number_generator::instance().random_int(min_particle_alpha, max_particle_alpha),
				// velocity
				sf::Vector2f(
					util::random_number_generator::instance().random_real<float>(min_particle_speed, max_particle_speed),
					util::random_number_generator::instance().random_real<float>(min_particle_speed, max_particle_speed)
				),
				// thisptr
				this
			);
		}
	}

	particle_system::~particle_system() {
		m_particles.clear();
	}

	void particle_system::draw() {
		if (m_particles.empty())
			return;

		for (auto &particle : m_particles) {
			particle.simulate();
			particle.draw();
			find_all_connectors(particle);
		}
	}

	void particle_system::update_particle_count(size_t new_count) {
		if (m_particle_count == new_count)
			return;

		m_particle_count = new_count;
		m_particles.resize(m_particle_count);
	}

	void particle_system::particle::draw() const {
		g_app->draw_circle(m_position, particle_radius, [this]() {
			auto clr = g_app->theme().accent_color;
			clr.a = m_alpha;
			return clr;
		}());
	}

	void particle_system::find_all_connectors(const particle &from) {
		for (const auto &particle : m_particles) {
			const auto distance = std::abs(particle.position().x - from.position().x) + std::abs(particle.position().y - from.position().y);
			if (distance < max_connection_distance)
				particle.draw_connection(from);
		}
	}

	void particle_system::particle::simulate() {
		// move away from mouse
		constexpr auto mouse_affect_range = 18.f;
		auto mouse = g_app->actual_mouse_pos();
		if (std::abs(mouse.x - m_position.x) < mouse_affect_range || std::abs(mouse.y - m_position.y) < mouse_affect_range) {
			// hacky but stops particles from jittering when close to mouse
			auto speedup = [](float &value) {
				if (value > 0.f)
					value += util::random_number_generator::instance().random_real<float>(0.f, particle_speedup);
				else
					value += util::random_number_generator::instance().random_real<float>(-particle_speedup, 0.f);
			};

			speedup(m_velocity.x);
			speedup(m_velocity.y);
		}

		// slow down
		constexpr auto slowdown_rate = 0.2f;
		auto slowdown = [slowdown_rate](float &component) {
			if (std::abs(component) > max_particle_speed) {
				if (component > 0.f) {
					component -= slowdown_rate;
					component = std::max(component, max_particle_speed);
				} else {
					component += slowdown_rate;
					component = std::min(component, min_particle_speed);
				}
			}
		};

		slowdown(m_velocity.x);
		slowdown(m_velocity.y);

		// bounce them off screen bounds
		auto next_position = get_simulated_position();
		if (next_position.x < 0.f || next_position.x > g_app->window()->getSize().x)
			m_velocity.x = -m_velocity.x;

		if (next_position.y < 0.f || next_position.y > g_app->window()->getSize().y)
			m_velocity.y = -m_velocity.y;

		m_position += m_velocity;
	}

	void particle_system::particle::draw_connection(const particle &to) const {
		g_app->draw_line(m_position, to.m_position, [this, to]() {
			auto clr = g_app->theme().accent_color;
			clr.a = std::min(m_alpha, to.m_alpha);
			return clr;
		}());
	}

	sf::Vector2f particle_system::particle::get_simulated_position() const {
		return sf::Vector2f(m_position.x + m_velocity.x, m_position.y + m_velocity.y);
	}
}