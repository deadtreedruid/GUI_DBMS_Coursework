#include "textbox.h"
#include "..\gui.h"

namespace gui {
	namespace element {
		void textbox::draw() {
			if (m_hidden)
				return;

			// outline
			g_app->draw_rect_outline(m_pos, m_size, m_focused ? g_app->theme().accent_color : g_app->theme().control_outline_color);

			// name
			g_app->draw_string(m_name, g_app->theme().text_size, sf::Vector2f(m_pos.x + 1.f, m_pos.y - (float)g_app->theme().text_size - 8.f), g_app->theme().text_color);

			// scale down font so that text doesn't overflow the bounds
			auto size = g_app->theme().text_size;
			while (g_app->string_bounds(m_value, size).x > m_size.x)
				--size;

			if (!m_value.isEmpty())
				g_app->draw_string(m_value.toAnsiString().data(), size, sf::Vector2f(m_pos.x + 1.f, m_pos.y + 3.f), g_app->theme().text_color);
		}

		void textbox::handle_event(sf::Event &evt) {
			if (m_hidden) {
				m_focused = false;
				return;
			}

			auto is_click_evt = evt.type == sf::Event::MouseButtonPressed;

			m_hovered = g_app->mouse_hovering(m_pos, m_size);
			m_clicked = is_click_evt && m_hovered;

			if (!m_focused && m_clicked)
				m_focused = true;
			else if (m_focused && is_click_evt && !m_hovered)
				m_focused = false;

			if (!m_focused)
				return;

			if (evt.type == sf::Event::KeyPressed) {
				switch (evt.key.code) {
				case sf::Keyboard::Escape:
				case sf::Keyboard::Delete:
					m_focused = false;
					m_value.clear();
					return;
					break;
				case sf::Keyboard::Enter:
				case sf::Keyboard::Tab:
					m_focused = false;
					break;
				case sf::Keyboard::Backspace:
					if (!m_value.isEmpty())
						m_value.erase(m_value.getSize() - 1u);
					return;
					break;
				default:
					break;
				}
			} else if (evt.type == sf::Event::TextEntered && evt.key.code != sf::Keyboard::Key(8)) {
				m_value += evt.text.unicode;
			}			
		}
	}
}