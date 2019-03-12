#include "resource_managers.h"
#include <iostream>

namespace gui {
	void image_manager::init() {
		m_resource_path = std::experimental::filesystem::v1::current_path().append("/resources/images/");

		create("logo.png");
	}

	void audio_manager::init() {
		m_resource_path = std::experimental::filesystem::v1::current_path().append("/resources/audio/");
	}

	void font_manager::init() {
		m_resource_path = std::experimental::filesystem::v1::current_path().append("/resources/fonts/");
		
		create("segoeui.ttf");
	}

	template<typename T>
	void resource_manager<T>::create(const std::string &name) {
		resource_type resource;
		auto find = name.find(".");
		if (find == std::string::npos) {
		#ifdef _DEBUG
			printf("Resource file %s has no extension.\n", name.data());
		#endif
			return;
		}

		if (resource.loadFromFile(m_resource_path.string().append(name))) {
			auto mapname = name.substr(0u, find);
			m_resources[util::hash(mapname)()] = resource;
		} else {
		#ifdef _DEBUG
			printf("Failed to load file %s\n", name.data());
		#endif
		}
	}

	sf::Sound audio_manager::play_sound(sf::SoundBuffer *buffer, float volume, bool looping) {
		if (!buffer)
			return sf::Sound{};

		auto sound = sf::Sound(*buffer);
		sound.setVolume(volume);
		sound.setLoop(looping);
		return std::move(sound);
	}

	sf::Sound audio_manager::play_sound(util::hash::hash_t name, float volume, bool looping) {
		auto buffer = get_resource(name);
		auto sound = sf::Sound(buffer);
		sound.setVolume(volume);
		sound.setLoop(looping);
		return std::move(sound);
	}
}

gui::audio_manager *g_audio;
gui::font_manager *g_fonts;
gui::image_manager *g_images;