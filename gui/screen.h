#ifndef SCREEN_H
#define SCREEN_H
#include <vector>

#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>
#include <SFML\System.hpp>
#include <SFML\Window.hpp>

#include "../typedefs.h"

namespace gui {
	namespace element { class base; }

	class screen {
	public:
		virtual ~screen();
		virtual void draw() = 0;
		virtual void handle_event(sf::Event &event) = 0;

		template<typename T, bool Hidden = false, typename ...Ts>
		inline auto add_gui_element(const Ts &...args) {
			static_assert(std::is_base_of_v<element::base, T>, "T must be element type");
			m_elems.emplace_back(new T(args...));
			m_elems.back()->m_screen = this;
			m_elems.back()->m_hidden = Hidden;
			return (T *)m_elems.back();
		}

		template<typename T = screen>
		inline const T *get_parent() const {
			static_assert(std::is_base_of_v<screen, T>, "screen::get_parent() -> T must be screen type");
			return (T *)m_parent;
		}

		template<typename T = screen>
		inline T *get_parent() {
			static_assert(std::is_base_of_v<screen, T>, "T must be screen type");
			return (T *)m_parent;
		}

		template<typename T = element::base>
		inline T *get_element(size_t index) {
			static_assert(std::is_base_of_v<element::base, T>, "T must be element type");
			if (index >= m_elems.size())
				return nullptr;
			return (T *)m_elems[index];
		}

		template<typename T = element::base>
		inline const T *get_element(size_t index) const {
			static_assert(std::is_base_of_v<element::base, T>, "T must be element type");
			if (index >= m_elems.size())
				return nullptr;
			return (T *)m_elems[index];
		}

		inline void focus_on(element::base *ptr) { m_focus = ptr; }
		inline void lose_focus() { m_focus = nullptr; }
		inline const element::base *currently_focused() const { return m_focus; }
		inline const char *name() const { return m_name; }
		inline void set_parent(screen *parent) { m_parent = parent; }
	protected:
		screen *m_parent = nullptr;
		std::vector<element::base *> m_elems{};
		element::base *m_focus{};
		const char *m_name{};
	};

	// this is messy but it makes code so much neater
	class screen_subscreenable : public screen {
	public:
		virtual ~screen_subscreenable();

		template<typename SubscreenType>
		inline auto add_subscreen() {
			static_assert(std::is_base_of_v<screen, SubscreenType>, "screen_subscreenable::add_subscreen -> SubscreenType must be valid screen");
			m_subscreens.push_back(new SubscreenType());
			m_subscreens.back()->set_parent(this);
			return (SubscreenType *)m_subscreens.back();
		}

		template<typename T = screen>
		inline T *get_subscreen(size_t index) {
			static_assert(std::is_base_of_v<screen, T>, "T must be screen type");
			if (index >= m_subscreens.size())
				return nullptr;

			return (T *)m_subscreens[index];
		}

		template<typename T = screen>
		inline const T *get_subscreen(size_t index) const {
			static_assert(std::is_base_of_v<screen, T>, "T must be screen type");
			if (index >= m_subscreens.size())
				return nullptr;

			return (T *)m_subscreens[index];
		}

		inline void to_root() { m_active_subscreen = nullptr; }
		inline const auto active_subscreen() const { return m_active_subscreen; }
		inline void goto_subscreen(screen *subscr) { m_active_subscreen = subscr; m_active_subscreen->lose_focus(); this->lose_focus(); }
	protected:
		std::vector<screen *> m_subscreens{};
		screen *m_active_subscreen = nullptr;
	};
} // namespace gui

#endif // SCREEN_H