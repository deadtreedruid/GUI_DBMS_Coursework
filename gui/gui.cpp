#include "gui.h"
#include "resource_managers.h"
#include "main_menu.h"
#include "student_options.h"
#include "course_options.h"
#include "enrol.h"

#define USE_ANTIALIASING 1
#define AA_LEVEL 4

namespace gui {
	// sf::Color doesn't support constexpr for whatever reason
	static const auto dark_theme = app_theme(
		// background
		sf::Color(53, 53, 53),
		// accent
		sf::Color(211, 9, 57, 255),
		// text
		sf::Color(240, 240, 240),
		// control outline
		sf::Color(210, 210, 210, 200),
		// warning success
		sf::Color(70, 255, 70, 255),
		// warning fail
		sf::Color(255, 70, 70, 255),
		// ui text size
		18,
		// warning text size
		22
	);

	static const auto light_theme = app_theme(
		// background
		sf::Color(255, 255, 255),
		// accent
		sf::Color(240, 9, 57, 255),
		// text
		sf::Color(60, 60, 60),
		// control outline
		sf::Color(0, 0, 0, 255),
		// warning success
		sf::Color(20, 255, 20, 255),
		// warning fail
		sf::Color(255, 20, 20, 255),
		// ui text size
		18,
		// warning text size
		22
	);

	application::application(u32 target_w, u32 target_h) {
		auto videomode = sf::VideoMode::getDesktopMode();
		videomode.width = std::min(target_w, videomode.width);
		videomode.height = std::min(target_h, videomode.height);

		sf::ContextSettings ctx;
	#if USE_ANTIALIASING
		ctx.antialiasingLevel = AA_LEVEL;
	#endif

		m_window = std::make_unique<sf::RenderWindow>(
			videomode,
			"Liverpool Hope Student Manager",
			sf::Style::Titlebar | sf::Style::Close,
			ctx
		);

		m_theme = dark_theme;

		g_fonts = new font_manager();
		g_fonts->init();
		g_audio = new audio_manager();
		g_audio->init();
		g_images = new image_manager();
		g_images->init();

		// have to use the lambda because of an MSVC bug where compile time hashes initialised in an arg list only has its first byte hashed
		auto logo = g_images->get_resource([]() { constexpr auto ret = util::hash::compile_time("logo"); return ret; }());
		m_window->setIcon(300, 366, logo.getPixelsPtr());
	}

	void application::run() {
		// init screens
		auto menu = new main_menu();
		m_particles = std::make_unique<particle_system>();

		sf::Clock clock;
		// render loop
		while (m_window->isOpen()) {
			m_frametime = (float)clock.getElapsedTime().asSeconds();
			m_framerate = 1.f / m_frametime;
			clock.restart();

			// handle events
			sf::Event evt;
			while (m_window->pollEvent(evt)) {
				if (evt.type == evt.Closed)
					exit();
				else
					menu->handle_event(evt);
			}

			m_window->clear();
			{
				// background
				draw_gradient({ 0.f, 0.f }, m_window->getDefaultView().getSize(), m_theme.bg_color, [this]() {
					auto clr = m_theme.bg_color;
					gradient_fade_helper::subtract(clr.r, 30);
					gradient_fade_helper::subtract(clr.g, 30);
					gradient_fade_helper::subtract(clr.b, 30);
					return clr;
				}());

				if (m_should_draw_particles)
					m_particles->draw();

				menu->draw();
				
				// draw any warnings that we have
				if (!m_warning_queue.empty())
					draw_warning();
			}
			m_window->display();
			++m_framecount;
		}

		safe_delete(menu);
	}

	void application::exit() {
		m_window->close();
	}

	sf::Vector2f application::actual_mouse_pos() const {
		return m_window->mapPixelToCoords(sf::Mouse::getPosition(*m_window));
	}

	bool application::mouse_hovering(const sf::Vector2f &pos, const sf::Vector2f &size) const {
		auto mouse = actual_mouse_pos();
		return sf::FloatRect(pos, size).contains(mouse);
	}

	sf::Vector2f application::string_bounds(const sf::String &str, u32 size, draw_fonts _font) const {
		sf::Font font;
		switch (_font) {
		case FONT_SEGOEUI:
			font = g_fonts->get_resource(util::hash("segoeui")());
			break;
		default:
			break;
		}

		sf::Text txt(str, font, size);
		// NOTE: must be global not local bounds in case we apply screen transformations
		auto gbounds = txt.getGlobalBounds();
		return sf::Vector2f(gbounds.width, gbounds.height);
	}

	void application::draw_rect(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &clr, bool outlined) {
		sf::RectangleShape rect;
		rect.setPosition(pos);
		rect.setSize(size);
		rect.setFillColor(clr);

		if (outlined) {
			rect.setOutlineColor(sf::Color::Black);
			rect.setOutlineThickness(1.f);
		}

		m_window->draw(rect);
	}

	void application::draw_gradient(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &a, const sf::Color &b) {
		sf::VertexArray rect(sf::TriangleStrip); 
		rect.append(sf::Vertex(pos, a));
		rect.append(sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y), a));
		rect.append(sf::Vertex(sf::Vector2f(pos.x, pos.y + size.y), b));
		rect.append(sf::Vertex(sf::Vector2f(pos.x + size.x, pos.y + size.y), b));

		m_window->draw(rect);
	}

	void application::draw_string(const char *str, u32 size, const sf::Vector2f &pos, const sf::Color &clr, bool centered, bool outlined, draw_fonts _font) {
		if (!str || str[0] == '\0')
			return;

		sf::Font font;
		switch (_font) {
		case FONT_SEGOEUI:
			font = g_fonts->get_resource(util::hash("segoeui")());
			break;
		default:
			break;
		}

		sf::Text text;
		text.setString(str);
		text.setCharacterSize(size);
		// todo: more options
		text.setFont(font);
		text.setFillColor(clr);

		if (outlined) {
			text.setOutlineColor(sf::Color::Black);
			text.setOutlineThickness(1.f);
		}

		if (centered) {
			auto bounds = text.getLocalBounds();
			sf::Vector2f centered(pos.x - (bounds.left + bounds.width) * 0.5f, pos.y);
			text.setPosition(centered);
		} else {
			text.setPosition(pos);
		}

		m_window->draw(text);
	}

	void application::draw_line(const sf::Vector2f &src, const sf::Vector2f &dst, const sf::Color &clr) {
		sf::VertexArray verts(sf::Lines, 2);
		verts.append({ sf::Vector2f( src.x, src.y ), clr });
		verts.append({ sf::Vector2f( dst.x, dst.y ), clr });
		m_window->draw(verts);
	}

	void application::draw_rect_outline(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Color &clr) {
		sf::VertexArray verts(sf::LineStrip);
	
		/*
			Verticies should be as follows:
			Must be counter-clockwise to prevent bug where a line is drawn from 0,0

			(0|4)        <-3

			|              ^
			v			   |
			1->            2
		*/

		verts.append({ sf::Vector2f(pos.x, pos.y), clr });
		verts.append({ sf::Vector2f(pos.x, pos.y + size.y), clr });
		verts.append({ sf::Vector2f(pos.x + size.x, pos.y + size.y), clr });
		verts.append({ sf::Vector2f(pos.x + size.x, pos.y), clr });
		verts.append({ sf::Vector2f(pos.x, pos.y), clr });
		m_window->draw(verts);
	}

	void application::draw_triangle(const sf::Vector2f &a, const sf::Vector2f &b, const sf::Vector2f &c, const sf::Color &clr) {
		sf::VertexArray verts(sf::Triangles);
		verts.append({ a, clr });
		verts.append({ b, clr });
		verts.append({ c, clr });
		m_window->draw(verts);
	}
	
	void application::draw_circle(const sf::Vector2f &center, float radius, const sf::Color &clr) {
		sf::CircleShape shape;
		shape.setPosition(center);
		shape.setRadius(radius);
		shape.setFillColor(clr);
		m_window->draw(shape);
	}

	// Exceptions are slow and bad
	void application::push_warning(const std::string &msg, float duration, warning_status status) {
		// if we have too many warnings just get rid of the older ones
		constexpr size_t max_warnings = 12;
		while (m_warning_queue.size() > max_warnings)
			m_warning_queue.pop_front();

		m_warning_queue.emplace_back(
			msg,
			std::numeric_limits<u8>::max(),
			duration,
			status,
			m_framecount
		);
	}

	void application::draw_warning() {
		auto &cur_warning = m_warning_queue.front();
		// current warning has expired
		if (cur_warning.alpha == std::numeric_limits<u8>::min() || cur_warning.text.empty()) {
			m_warning_queue.pop_front();

			if (m_warning_queue.empty())
				return;
			
			// assign next warning we need to handle
			cur_warning = m_warning_queue.front();
		}

		const auto center_x = (float)g_app->window()->getSize().x * 0.5f;
		const auto screen_y = (float)g_app->window()->getSize().y;

		g_app->draw_string(cur_warning.text.data(), m_theme.warning_size, sf::Vector2f(center_x, screen_y * 0.2f), g_app->theme().get_warning_color(cur_warning.alpha, cur_warning.status == WARNING_FAIL), true);

		// maintain max alpha until duration is reached
		auto delta = float(m_framecount - cur_warning.framecount);
		auto time_since_issue = delta * std::max(m_frametime, 0.01f);
		if (time_since_issue <= cur_warning.duration)
			return;

		// fade out
		const auto fade_seconds = cur_warning.duration * 0.3f;
		auto fade_time = 255.f / fade_seconds;
		auto deduct = u8(fade_time * std::max(m_frametime, 0.01f));
		if (deduct > cur_warning.alpha)
			cur_warning.alpha = std::numeric_limits<u8>::min();
		else
			cur_warning.alpha -= deduct;

		util::Clamp(cur_warning.alpha, std::numeric_limits<u8>::min(), std::numeric_limits<u8>::max());
	}

	void application::change_theme() {
		if (m_using_dark_theme)
			m_theme = light_theme;
		else
			m_theme = dark_theme;
		m_using_dark_theme = !m_using_dark_theme;
	}
}

gui::application *g_app = nullptr;