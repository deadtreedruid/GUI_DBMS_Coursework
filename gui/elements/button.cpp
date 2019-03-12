#include "button.h"
#include "..\gui.h"
#include "..\..\util\math.h"

namespace gui { 
	namespace element {
		void button::draw() {
			constexpr auto fade_seconds = 0.5f;
			constexpr auto fade_rate = 255.f / 0.5f;
			constexpr auto min_alpha = std::numeric_limits<u8>::min();
			constexpr auto max_alpha = u8(150);

			if (m_hidden) {
				m_hover_opacity = min_alpha;
				return;
			}

			g_app->draw_rect_outline(m_pos, m_size, g_app->theme().control_outline_color);

			// fade in/out when hovered
			// uses app frametime to make sure animation speed is always constant regardless of framerate
			const auto modifier = u8(fade_rate * std::max(g_app->frametime(), 0.01f));
			if (m_hovered && m_hover_opacity < max_alpha) {
				if (modifier > (max_alpha - m_hover_opacity))
					m_hover_opacity = max_alpha;
				else 
					m_hover_opacity += modifier;
			} else if (!m_hovered && m_hover_opacity > min_alpha) {
				if (modifier > m_hover_opacity)
					m_hover_opacity = min_alpha;
				else 
					m_hover_opacity -= modifier;
			}
			util::Clamp(m_hover_opacity, min_alpha, max_alpha);

			constexpr auto padding = 0.f, padding2 = padding * 2.f;
			g_app->draw_rect(sf::Vector2f(m_pos.x + padding, m_pos.y + padding), sf::Vector2f(m_size.x - padding2, m_size.y - padding2), [this]() {
				auto clr = sf::Color(g_app->theme().accent_color);
				clr.a = m_hover_opacity;
				return clr;
			}());

			g_app->draw_string(m_name, g_app->theme().text_size, sf::Vector2f(m_pos.x + m_size.x * 0.5f, m_pos.y + (m_size.y * 0.5f) - ((float)g_app->theme().text_size * 0.5f) - 2.f), g_app->theme().text_color, true);
		}

		void button::handle_event(sf::Event &evt) {
			if (m_hidden)
				return;

			m_hovered = g_app->mouse_hovering(m_pos, m_size);
			if (!m_hovered)
				return;

			m_clicked = false;

			switch (evt.type) {
			case sf::Event::MouseButtonPressed:
				m_clicked = true;
				break;
			case sf::Event::MouseButtonReleased:
				m_callback(this);
				break;
			default:
				break;
			}
		}
	} 
}