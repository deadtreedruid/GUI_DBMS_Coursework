#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H
#include "base.h"
#include <functional>
/*
Invokes a callback when pressed
*/

using u8 = unsigned char;

namespace gui {
	namespace element {
		class button : public base {
		public:
			using callback_t = std::function<void(button *)>;

			inline button(const char *name, const sf::Vector2f &pos, const sf::Vector2f &size, callback_t &callback, bool centered = false) : m_callback{ callback } { 
				m_name = name;
				m_size = size;
				if (centered)
					m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
				else
					m_pos = pos;
			}

			// lambda initialisation
			template<typename fn>
			inline button(const char *name, const sf::Vector2f &pos, const sf::Vector2f &size, fn &&callback, bool centered = false) : m_callback{ callback } { 
				m_name = name;
				m_size = size;
				if (centered)
					m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
				else
					m_pos = pos;
			}

			void draw() override;
			void handle_event(sf::Event &evt) override;
		protected:
			callback_t m_callback = nullptr;
			u8 m_hover_opacity{};
		};
	}
}

#endif // GUI_BUTTON_H