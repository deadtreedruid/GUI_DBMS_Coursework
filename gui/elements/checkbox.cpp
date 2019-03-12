#include "checkbox.h"
#include "..\gui.h"

namespace gui {
	namespace element {
		void checkbox::draw() {
			if (m_hidden)
				return;

			// draw the box itself
			constexpr auto box_padding = 8.f;
			auto fill_padding = box_padding * 0.5f;

			// shrink the box a bit when we're clicking down on it
			if (m_clicked)
				fill_padding *= 1.2f;

			const auto box_size = m_size.y - box_padding;
			const auto box_pos = sf::Vector2f(m_pos.x + m_size.x - box_size, m_pos.y);

			// name
			auto bounds = g_app->string_bounds(m_name, g_app->theme().text_size);
			auto size = g_app->theme().text_size;
			while (m_pos.x + g_app->string_bounds(m_name, size).x > box_pos.x)
				--size;

			sf::Vector2f text_pos(m_pos.x, m_pos.y + (m_size.y * 0.5f) - (float)size);
			g_app->draw_string(m_name, size, text_pos, g_app->theme().text_color);

			// outline
			g_app->draw_rect_outline(box_pos, sf::Vector2f(box_size, box_size), m_hovered ? g_app->theme().accent_color : g_app->theme().control_outline_color);

			// fill
			if (m_value)
				g_app->draw_rect({ box_pos.x + fill_padding, box_pos.y + fill_padding }, { box_size - fill_padding * 2.f, box_size - fill_padding * 2.f }, g_app->theme().accent_color);
			else if (!m_value && m_clicked) {
				g_app->draw_rect({ box_pos.x + fill_padding, box_pos.y + fill_padding }, { box_size - fill_padding * 2.f, box_size - fill_padding * 2.f }, []() {
					auto ret = g_app->theme().bg_color;
					gradient_fade_helper::add(ret.r, 30);
					gradient_fade_helper::add(ret.g, 30);
					gradient_fade_helper::add(ret.b, 30);
					return ret;
				}());
			}
		}

		void checkbox::handle_event(sf::Event &evt) {
			if (m_hidden)
				return;

			auto is_click_evt = evt.type == sf::Event::MouseButtonPressed;

			m_hovered = g_app->mouse_hovering(m_pos, m_size);
			m_clicked = is_click_evt && m_hovered;

			if (m_hovered && evt.type == sf::Event::MouseButtonReleased)
				m_value = !m_value;
		}
	}
}