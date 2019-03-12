#ifndef GUI_CHECKBOX_H
#define GUI_CHECKBOX_H
#include "base.h"

namespace gui {
	namespace element {
		class checkbox : public value_base<bool> {
		public:
			inline checkbox(const char *str, const sf::Vector2f &pos, const sf::Vector2f &size, bool centered = false, bool default_value = false) {
				m_name = str;
				if (centered)
					m_pos = sf::Vector2f(pos.x - size.x * 0.5f, pos.y);
				else
					m_pos = pos;
				m_size = size;
				m_value = default_value;
			}

			void draw() override;
			void handle_event(sf::Event &evt) override;
		};
	}
}

#endif // GUI_TEXTBOX_H