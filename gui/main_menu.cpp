#include "main_menu.h"
#include "gui.h"
#include "student_options.h"
#include "course_options.h"
#include "enrol.h"

#include "elements\button.h"
#include "elements\slider.h"

namespace gui {
	main_menu::main_menu() {
		setup_controls();
	}

	void main_menu::draw() {
		if (m_changed_themes) {
			get_element(4u)->set_name(g_app->get_theme_name());
			m_changed_themes = false;
		}

		if (m_toggled_particles) {
			get_element(5u)->set_name(g_app->get_particle_state());
			m_toggled_particles = false;
		}

		// WAY too expensive, stick with toggle instead
		/*static size_t last_particle_count = std::numeric_limits<size_t>::max();
		if (auto particle_count = get_element<element::slider<int>>(5u)->get(); particle_count != last_particle_count) {
			g_app->update_particle_count(particle_count);
			last_particle_count = particle_count;
		}*/

		if (!m_active_subscreen) {
			const auto title_x = (float)g_app->window()->getSize().x * 0.5f;
			float y_offset = (float)g_app->window()->getSize().y * 0.2f;

			g_app->draw_string("Liverpool Hope University", 40, { title_x, y_offset - 55.f }, g_app->theme().text_color, true);
			g_app->draw_string("Student Database Manager", 40, { title_x, y_offset }, g_app->theme().text_color, true);
			y_offset += 55.f;

			g_app->draw_string("Main Menu", 25, { title_x, y_offset }, g_app->theme().text_color, true);

			for (auto elem : m_elems) {
				if (m_focus != elem)
					elem->draw();
			}

			if (m_focus)
				m_focus->draw();
		} else {
			m_active_subscreen->draw();
			for (size_t i{ 4 }; i < m_elems.size(); ++i)
				get_element(i)->draw();
		}
	}

	void main_menu::handle_event(sf::Event &evt) {
		if (!m_active_subscreen) {
			if (m_focus)
				m_focus->handle_event(evt);
			else {
				for (auto elem : m_elems)
					elem->handle_event(evt);
			}
		} else {
			m_active_subscreen->handle_event(evt);
			for (size_t i{ 4 }; i < m_elems.size(); ++i)
				get_element(i)->handle_event(evt);
		}
	}

	void main_menu::setup_controls() {
		add_subscreen<student_options>();
		add_subscreen<course_options>();
		add_subscreen<enrol_options>();

		const auto title_x = (float)g_app->window()->getSize().x * 0.5f;
		const auto title_y = (float)g_app->window()->getSize().y * 0.2f;
		constexpr float height = 30.f;
		constexpr float spacing = height + 10.f;
		float y_offset = title_y + 145.f;

		add_gui_element<element::button>("Student Options", sf::Vector2f(title_x, y_offset), sf::Vector2f(180.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[0]);
		}, true);

		y_offset += spacing;

		add_gui_element<element::button>("Course Options", sf::Vector2f(title_x, y_offset), sf::Vector2f(180.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[1]);
		}, true);

		y_offset += spacing;

		add_gui_element<element::button>("Enrol Options", sf::Vector2f(title_x, y_offset), sf::Vector2f(180.f, height), [&](element::button *btn) {
			goto_subscreen(m_subscreens[2]);
		}, true);

		y_offset += spacing + 30.f;

		add_gui_element<element::button>("Exit", sf::Vector2f(30.f, (title_y * 5.f) - 50.f), sf::Vector2f(100.f, height), [&](element::button *btn) {
			g_app->exit();
		});

		add_gui_element<element::button>("Dark theme", sf::Vector2f(g_app->window()->getSize().x - 30.f - 130.f, (title_y * 5.f) - 50.f), sf::Vector2f(130.f, height), [&](element::button *btn) {
			g_app->change_theme();
			m_changed_themes = true;
		});

		add_gui_element<element::button>("Particles: On", sf::Vector2f(g_app->window()->getSize().x - 60.f - 260.f, (title_y * 5.f) - 50.f), sf::Vector2f(130.f, height), [&](element::button *) {
			g_app->toggle_particles();
			m_toggled_particles = true;
		});

		//add_gui_element<element::slider<int>>("Particles", sf::Vector2f(g_app->window()->getSize().x - 90.f - 260.f - 256.f, (title_y * 5.f) - 50.f), sf::Vector2f(256.f, height), 0, 128);
	}
}