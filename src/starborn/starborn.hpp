/*
 * Copyright (C) 2013-2015 Snailsoft <https://github.com/snailsoft/starborn/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <apathy/apathy.hpp>
#include <base91/base91.hpp>
#include <bubble/bubble.hpp>
#include <bundle/bundle.hpp>
#include <cocoa/cocoa.hpp>
#include <flow/flow.hpp>
#include <git.hpp>
#include <heal/heal.hpp>
#include <metrics/metrics.hpp>
#include <profit/profit.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <sand/sand.hpp>

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

#include <Thor/Animations.hpp>
#include <Thor/Input.hpp>
#include <Thor/Resources.hpp>

#include <wire/wire.hpp>

#include <starborn/constants.hpp>

#include <starborn/sprite.hpp>
#include <starborn/animated_sprite.hpp>
#include <starborn/animated_string.hpp>
#include <starborn/structs.hpp>

#include <starborn/rectangle.hpp>
#include <starborn/animated_rectangle.hpp>

#include <starborn/functions.hpp>
#include <starborn/json.hpp>
#include <starborn/menu.hpp>
#include <starborn/state.hpp>

#include <starborn/version.hpp>

namespace ss
{
	class Starborn
	{
		private:
			float music_volume;
			float sound_volume;

			sf::Music title_music;
			sf::Music world_music;

			sf::RenderWindow window;
			sf::View view;

			State state;

			std::map<wire::string, Animation> animations;
			std::map<wire::string, Menu> menus;

			std::map<wire::string, sf::Shader> shaders;
			std::map<wire::string, std::pair<wire::string, thor::ResourceHolder<sf::Font, wire::string>>> fonts;

			std::map<wire::string, wire::string> raw_music;
			std::map<wire::string, wire::string> shader_sources;

			std::thread loading_thread;
			std::vector<sf::Sound *> sounds;
			
			thor::ActionMap<wire::string> actions;
			thor::ActionMap<wire::string>::CallbackSystem callbacks;

			thor::ResourceHolder<sf::Texture, wire::string> textures;
			thor::ResourceHolder<sf::SoundBuffer, wire::string> raw_sounds;

			void clean_sounds();
			void fade_sounds(bool music = false);

			void load(std::vector<wire::string> &critical_files);

			void load_animations(bundle::string &json_data);
			void load_drawables(bundle::string &json_data);
			void load_shaders(bundle::string &json_data);

			void new_game(bool midnight = true);

			void on_continue();
			void on_down();
			void on_escape();
			void on_exit();
			void on_left();
			void on_right();
			void on_screenshot();
			void on_select();
			void on_up();

		public:
			sf::Music &get_title_music();
			sf::Music &get_world_music();

			~Starborn();
			Starborn();

			State &get_state();

			void play_sound(wire::string filename, bool music = false);
			void run();
	};
}
