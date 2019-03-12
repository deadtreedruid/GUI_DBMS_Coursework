#ifndef GUI_DROPDOWN_H
#define GUI_DROPDOWN_H
#include "base.h"
#include "..\gui.h"
#include "..\..\util\fnv.h"

namespace gui {
	namespace element {
		using names_t = std::vector<const char *>;

		class dropdown : public value_base<size_t> {
		public:
			inline dropdown(const char *name, const sf::Vector2f &pos, const sf::Vector2f &size, names_t names, bool centered = false);
			inline void draw() override;
			inline void handle_event(sf::Event &) override;
			inline void alter_names(names_t new_names);
			inline bool has_names() const { return !m_names.empty(); }

		protected:
			bool m_open = false;
			std::vector<const char *> m_names{};
			std::vector<bool> m_hoverings = { false };
		};

		dropdown::dropdown(const char *name, const sf::Vector2f &pos, const sf::Vector2f &size, names_t names, bool centered) {
			m_name = name;
			if (centered)
				m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
			else
				m_pos = pos;
			m_size = size;
			m_names = std::move(names);
			m_hoverings.resize(m_names.size());
		}

		void dropdown::draw() {
			if (m_hidden)
				return;

			// give a 0.2s buffer after we close to unfocus
			constexpr auto buffer_seconds = 0.2f;
			auto focus_frame_buffer = g_app->framerate();
			static size_t unfocus_ticks{};
			if (m_open && m_screen->currently_focused() != this) {
				m_screen->focus_on(this);
			#pragma warning(push)
			#pragma warning(disable:4244)
				unfocus_ticks = std::round<size_t>(focus_frame_buffer * buffer_seconds);
			#pragma warning(pop)
			} else if (!m_open && m_screen->currently_focused() == this) {
				if (unfocus_ticks)
					--unfocus_ticks;
				else
					m_screen->lose_focus();
			}

			// name
			g_app->draw_string(m_name, g_app->theme().text_size, sf::Vector2f(m_pos.x + 1.f, m_pos.y - (float)g_app->theme().text_size - 8.f), g_app->theme().text_color);

			auto drop_indicator_size = m_size.x * 0.1f;
			constexpr float vertical_pad = 8.f, horizontal_pad = 5.f;

			auto scale_font = [&](const char *name, float max_size) {
				auto size = g_app->theme().text_size;
				while (g_app->string_bounds(name, size).x > (max_size - horizontal_pad * 2.f - drop_indicator_size))
					--size;

				return size;
			};

			// draw selected item
			g_app->draw_rect_outline(m_pos, m_size, (m_hovered || m_open) ? g_app->theme().accent_color : g_app->theme().control_outline_color);
			if (m_names.empty())
				return;

			const auto &name = m_names[m_value];
			auto size = scale_font(name, m_size.x);

			constexpr auto text_padding = 3.f;
			g_app->draw_string(name, size, sf::Vector2f(m_pos.x + text_padding, m_pos.y + text_padding), g_app->theme().text_color);

			// draw a triangle at the end of the box to indicate to the user that it's a dropdown
			g_app->draw_triangle(
				{ m_pos.x + (m_size.x - drop_indicator_size - horizontal_pad), m_open ? m_pos.y + m_size.y - vertical_pad : m_pos.y + vertical_pad }, 
				{ m_pos.x + (m_size.x - horizontal_pad), m_open ? m_pos.y + m_size.y - vertical_pad : m_pos.y + vertical_pad }, 
				{ m_pos.x + (m_size.x - horizontal_pad - (drop_indicator_size * 0.5f)), m_open ? m_pos.y + vertical_pad : m_pos.y + m_size.y - vertical_pad }, 
				m_open ? g_app->theme().accent_color : g_app->theme().control_outline_color
			);

			// expanded container
			if (m_open) {
				float y_full{};
				size_t i{};
				for (const auto &name : m_names) {
					y_full += m_size.y;
					size = scale_font(name, m_size.x - 2.f);
					bool hovered = m_hoverings[i];

					auto gradclr = hovered ? g_app->theme().accent_color : g_app->theme().bg_color;

					// gradient for each dropdown item to make it easier to see them
					g_app->draw_gradient(
						sf::Vector2f(m_pos.x + 1.f, m_pos.y + y_full), 
						sf::Vector2f(m_size.x - 2.f, m_size.y), 
						gradclr,
						[hovered, gradclr]() {
							auto clr = gradclr;
							u8 sub = hovered ? 50 : 30;
							gradient_fade_helper::subtract(clr.r, sub);
							gradient_fade_helper::subtract(clr.g, sub);
							gradient_fade_helper::subtract(clr.b, sub);
							return clr;
						}()
					);

					g_app->draw_string(name, size, sf::Vector2f(m_pos.x + text_padding, m_pos.y + y_full + text_padding), g_app->theme().text_color);
					++i;
				}

				g_app->draw_rect_outline(sf::Vector2f(m_pos.x, m_pos.y + m_size.y), sf::Vector2f(m_size.x, y_full), g_app->theme().control_outline_color);
			}
		}

		void dropdown::handle_event(sf::Event &evt) {
			if (m_hidden) {
				m_open = false;
				return;
			}

			auto is_click_evt = evt.type == sf::Event::MouseButtonPressed;

			m_hovered = g_app->mouse_hovering(m_pos, m_size);
			m_clicked = is_click_evt && m_hovered;
			if (m_clicked && !m_open) {
				m_open = true;
				return;
			}

			if (!m_open || m_names.empty())
				return;

			size_t i{};
			float area{};
			for (const auto &name : m_names) {
				m_hoverings[i] = false;
				if (g_app->mouse_hovering(sf::Vector2f(m_pos.x, m_pos.y + ((i + 1) * m_size.y)), m_size)) {
					m_hoverings[i] = true;

					if (is_click_evt) {
						m_value = i;
						m_open = false;
						return;
					}
				}

				++i;
				area += m_size.y;
			}

			if (is_click_evt && !g_app->mouse_hovering(sf::Vector2f(m_pos.x, m_pos.y + m_size.y), sf::Vector2f(m_size.x, area)))
				m_open = false;
		}

		void dropdown::alter_names(names_t new_names) {
			m_names = std::move(new_names);
			m_hoverings.resize(m_names.size());
			m_value = 0u;
		}
	}
}

#endif // GUI_DROPDOWN_H