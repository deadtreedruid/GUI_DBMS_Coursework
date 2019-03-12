#ifndef GUI_TEXTBOX_H
#define GUI_TEXTBOX_H
#include "base.h"
#include <SFML\System.hpp>

namespace gui {
	namespace element {
		class textbox : public value_base<sf::String> {
		public:
			inline textbox(const char *str, const sf::Vector2f &pos, const sf::Vector2f &size, bool centered = false) {
				m_name = str;
				if (centered)
					m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
				else
					m_pos = pos;
				m_size = size;
			}

			void draw() override;
			void handle_event(sf::Event &evt) override;

			inline void clear() {
				this->m_value.clear();
			}

		private:
			bool m_focused{};
		};
	}
}

#endif // GUI_TEXTBOX_H