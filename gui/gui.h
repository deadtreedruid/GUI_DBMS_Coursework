#ifndef GUI_H
#define GUI_H
#include <memory>
#include <map>
#include <functional>
#include <deque>

#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>

#include "../typedefs.h"
#include "particle_system.h"

namespace gui {
	namespace element { class button; }

	class screen;

	struct app_theme {
		sf::Color bg_color;
		sf::Color accent_color;
		sf::Color text_color;
		sf::Color control_outline_color;
		u32 text_size;
		u32 warning_size;
		sf::Color warning_success;
		sf::Color warning_fail;

		inline app_theme() = default;
		constexpr app_theme(
			const sf::Color &bg, 
			const sf::Color &accent,
			const sf::Color &text, 
			const sf::Color &ctrl_outline, 
			const sf::Color &warning_s, 
			const sf::Color &warning_f, 
			u32 txts, 
			u32 warns) : 
			bg_color{ bg }, 
			accent_color{ accent }, 
			text_color{ text }, 
			control_outline_color{ ctrl_outline },
			warning_success{ warning_s },
			warning_fail{ warning_f },
			text_size{ txts },
			warning_size{ warns } {

		}

		inline auto get_warning_color(u8 alpha, bool fail = true) const {
			sf::Color ret;
			if (fail)
				ret = warning_fail;
			else
				ret = warning_success;
			ret.a = alpha;
			return ret;
		}
	};

	class gradient_fade_helper {
	public:
		static inline void subtract(u8 &value, u8 sub) {
			if (value < sub)
				value = std::numeric_limits<u8>::min();
			else
				value -= sub;
		}

		static inline void add(u8 &value, u8 to_add) {
			if (((int)value + (int)to_add) > 255)
				value = std::numeric_limits<u8>::max();
			else
				value += to_add;
		}
	};

	enum draw_fonts : u8 {
		FONT_SEGOEUI
	};

	enum warning_status : u8 {
		WARNING_FAIL,
		WARNING_SUCCESS,
		WARNING_NONE
	};

	struct warning_t {
		std::string text;
		u8 alpha;
		float duration;
		warning_status status;
		int framecount;

		inline warning_t(const std::string &_text, u8 _alpha, float _dur, warning_status _status, int _framecount) :
			text{ _text },
			alpha{ _alpha },
			duration{ _dur },
			status{ _status },
			framecount{ _framecount } {

		}
	};

	class application {
		using evt_callback = std::function<void(sf::Event &)>;
	public:
		explicit application(u32 target_w, u32 target_h);
		void run();
		void exit();

		// drawing helpers
		void draw_rect(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &clr, bool outlined = false);
		void draw_gradient(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &a, const sf::Color &b);
		void draw_string(const char *str, u32 size, const sf::Vector2f &pos, const sf::Color &clr, bool centered = false, bool outlined = false, draw_fonts font = FONT_SEGOEUI);
		void draw_line(const sf::Vector2f &src, const sf::Vector2f &dst, const sf::Color &clr);
		void draw_rect_outline(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &clr);
		void draw_triangle(const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c, const sf::Color &clr);
		void draw_circle(const sf::Vector2f &center, float radius, const sf::Color &clr);

		// other utility
		bool mouse_hovering(const sf::Vector2f &pos, const sf::Vector2f &size) const;
		sf::Vector2f actual_mouse_pos() const;
		sf::Vector2f string_bounds(const sf::String &str, u32 size, draw_fonts font = FONT_SEGOEUI) const;
		void push_warning(const std::string &text, float duration = 0.9f, warning_status status = WARNING_FAIL);
		void change_theme();
		inline void update_particle_count(size_t new_count) { m_particles->update_particle_count(new_count); }

		inline const char *get_theme_name() const { return m_using_dark_theme ? "Dark theme" : "Light theme"; }
		inline const char *get_particle_state() const { return m_should_draw_particles ? "Particles: On" : "Particles: Off"; }
		inline void toggle_particles() { m_should_draw_particles = !m_should_draw_particles; }

		//accessors/mutators
		inline auto &window() { return m_window; }
		inline const auto &window() const { return m_window; }
		inline const auto &theme() const { return m_theme; }
		inline const auto &frametime() const { return m_frametime; }
		inline const auto &framerate() const { return m_framerate; }
	private:
		void draw_warning();

		app_theme m_theme;
		std::unique_ptr<sf::RenderWindow> m_window;
		std::unique_ptr<particle_system> m_particles;
		std::deque<warning_t> m_warning_queue{};
		float m_framerate{}, m_frametime{};
		int m_framecount{};
		bool m_using_dark_theme = true;
		bool m_should_draw_particles = true;
	};
} // namespace gui

extern gui::application *g_app;

#endif // GUI_H