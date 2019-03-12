#ifndef ELEMENT_BASE_H
#define ELEMENT_BASE_H
#include <vector>
#include <SFML\Window.hpp>
#include <SFML\System.hpp>
#include "..\..\typedefs.h"

namespace gui {
	class screen;
	class application;

	namespace element {
		class base {
			friend class screen;
		public:
			inline base() = default;
			inline ~base() { for (auto child : m_children) safe_delete(child); }
			virtual void draw() = 0;
			virtual void handle_event(sf::Event &event) = 0;

			inline void add_child(base *child) {
				child->m_parent = this;
				m_children.emplace_back(child);
			}

			inline void hide() { m_hidden = true; }
			inline void show() { m_hidden = false; }
			inline const auto &is_hidden() const { return m_hidden; }
			inline void reposition(const sf::Vector2f &new_pos) { m_pos = new_pos; }
			inline void resize(const sf::Vector2f &new_size) { m_size = new_size; }
			inline const auto &pos() const { return m_pos; }
			inline const auto &size() const { return m_size; }
			inline const auto &name() const { return m_name; }
			inline void set_name(const char *name) { m_name = name; }
		protected:
			const char *m_name = nullptr;
			bool m_hovered{}, m_clicked{};
			sf::Vector2f m_pos{}, m_size{};
			base *m_parent = nullptr;
			std::vector<base *> m_children{};
			screen *m_screen = nullptr;
			bool m_hidden = false;
		};

		template<typename T>
		class value_base : public base {
		public:
			inline value_base() : m_value{} { }
			inline value_base(const T &val) : m_value{ val } { }

			inline T &get() { return m_value; }
			inline const T &get() const { return m_value; }
			inline void set(const T &val) { m_value = val; }
			inline operator T() { return m_value; }
		protected:
			T m_value;
		};
	} // namespace element
} // namespace gui

#endif