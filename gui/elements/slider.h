#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H
#include "base.h"
#include "..\gui.h"
#include "..\..\util\math.h"

namespace gui {
	namespace element {
		template<typename T>
		class slider : public value_base<T> {
		public:
			inline slider(const char *name, const sf::Vector2f &pos, const sf::Vector2f &size, const T &min, const T &max, bool centered = false, T default_value = T{}, bool modifiable = true) {
				this->m_name = name;
				this->m_pos;
				this->m_size = size;
				m_min = min;
				m_max = max;
				this->set(default_value);

				if (centered)
					this->m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
				else
					this->m_pos = pos;

				m_modifiable = modifiable;
			}

			inline void draw() override;
			inline void handle_event(sf::Event &evt) override;

			inline void min_max(const T &min, const T &max, T *value = nullptr) {
				m_min = min;
				m_max = max;
				
				if (value)
					m_value = *value;
			}

		protected:
			bool m_should_update{}, m_modifiable{};
			T m_min{}, m_max{};
		};

		constexpr auto total_width_sub = 2.f;
		constexpr auto half_width_sub = total_width_sub * 0.5f;
		constexpr auto total_height_sub = 4.f;
		constexpr auto half_height_sub = total_height_sub * 0.5f;
	
		template<typename T>
		void slider<T>::draw() {
			if (this->m_hidden)
				return;

			// outline
			g_app->draw_rect_outline(this->m_pos, this->m_size, g_app->theme().control_outline_color);

			// draw bar if appropriate
			if (this->m_value != m_min) {
				auto width = float(this->m_value - m_min) * (this->m_size.x - total_width_sub) / float(m_max - m_min);
				g_app->draw_rect({ this->m_pos.x + half_width_sub, this->m_pos.y + half_height_sub }, { width, (this->m_size.y - total_height_sub + 1.f) }, g_app->theme().accent_color);
			}

			g_app->draw_string((std::string(this->m_name) + " (").append(std::_Floating_to_string("%.2f", [this]() {
				auto ratio = (float)m_value / (float)m_max;
				return ratio * 100.f;
			}()).append("%)")).data(), g_app->theme().text_size, { this->m_pos.x + 1.f, this->m_pos.y - (float)g_app->theme().text_size - 8.f }, g_app->theme().text_color);
		}

		template<typename T>
		void slider<T>::handle_event(sf::Event &evt) {
			if (this->m_hidden) {
				if (m_should_update)
					m_should_update = false;
				return;
			}

			this->m_hovered = g_app->mouse_hovering(this->m_pos, this->m_size);

			if (evt.type == sf::Event::MouseButtonPressed) {
				this->m_clicked = true;

				if (this->m_hovered)
					m_should_update = true;
			} else if (evt.type == sf::Event::MouseButtonReleased) {
				m_should_update = false;
				this->m_clicked = false;
			}

			if (!m_should_update && m_modifiable)
				return;

			auto drawn_width = this->m_size.x - total_width_sub;
			auto delta = g_app->actual_mouse_pos().x - this->m_pos.x - 1.f;
			util::Clamp(delta, 0.f, drawn_width);

			auto ratio = delta / drawn_width;
			this->m_value = T((float)m_min  + float(m_max - m_min) * ratio);
		}

		using sliderf = slider<float>;
	}
}

#endif // GUI_SLIDER_H