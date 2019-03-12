#ifndef RESOURCE_MANAGERS_H
#define RESOURCE_MANAGERS_H
#include <string>
#include <filesystem>
#include <unordered_map>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/System.hpp>

#include "..\util\fnv.h"

namespace gui {
	// holds resources of type T
	template<typename T>
	class resource_manager {
	public:
		using path_type = std::experimental::filesystem::v1::path;
		using resource_type = T;
		using ptr = T *;
		using ref = T &;

	public:
		virtual void init() = 0;

		inline resource_type get_resource(util::hash::hash_t name) const {
			if (!this)
				return T{};

			if (auto find = m_resources.find(name); find != m_resources.end())
				return find->second;
			return T{};
		}

	protected:
		void create(const std::string &full_file_name);
		path_type m_resource_path{};
		std::unordered_map<util::hash::hash_t, resource_type> m_resources{};
	};

	class audio_manager final : public resource_manager<sf::SoundBuffer> {
	public:
		void init() final;

		sf::Sound play_sound(sf::SoundBuffer *buffer, float volume, bool looping);
		sf::Sound play_sound(util::hash::hash_t name, float volume, bool looping);
	};

	class image_manager final : public resource_manager<sf::Image> {
	public:
		void init() final;
	};

	class font_manager final : public resource_manager<sf::Font> {
	public:
		void init() final;
	};
}

extern gui::audio_manager *g_audio;
extern gui::font_manager *g_fonts;
extern gui::image_manager *g_images;

#endif // RESOURCE_MANAGERS_H