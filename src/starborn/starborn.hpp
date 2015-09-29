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
#include <bundle/bundle.hpp>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <sand/sand.hpp>

#include <SFML/Graphics.hpp>

#include <Thor/Animations.hpp>
#include <Thor/Input.hpp>
#include <Thor/Resources.hpp>

#include <wire/wire.hpp>

#include <starborn/constants/actions.hpp>
#include <starborn/constants/anchors.hpp>
#include <starborn/constants/animation_types.hpp>
#include <starborn/constants/buttons.hpp>
#include <starborn/constants/drawable_types.hpp>
#include <starborn/constants/settings.hpp>
#include <starborn/constants/sprites.hpp>
#include <starborn/constants/sprite_types.hpp>
#include <starborn/constants/states.hpp>

#include <starborn/entities/sprite.hpp>
#include <starborn/entities/animated_sprite.hpp>

#include <starborn/resources/resources.hpp>
#include <starborn/game/state.hpp>

#include <starborn/ui/menu.hpp>
#include <starborn/utilities/json.hpp>

namespace ss
{
	namespace structs
	{
		struct Animation
		{
			sf::Time duration;
			std::function<void(sf::Sprite &, float)> animation;
		};
	};

	namespace maps
	{
		typedef std::map<wire::string, bundle::string> ShaderSources;
		typedef std::map<wire::string, sf::Shader> Shaders;
		typedef std::map<wire::string, structs::Animation> Animations;
		typedef std::map<wire::string, ui::Menu> Menus;
	};

	class Starborn
	{
		private:
			game::State state;

			maps::Animations animations;
			maps::Menus menus;
			maps::Shaders shaders;
			maps::ShaderSources shader_sources;

			resources::Textures textures;

			sf::RenderWindow window;
			sf::View view;

			thor::ActionMap<wire::string> actions;
			thor::ActionMap<wire::string>::CallbackSystem callbacks;

			bundle::string unpack_asset(bundle::file &asset);

			static void log(bool open, bool feed, bool close, const std::string &line);
			static wire::string get_filename(wire::string directory, wire::string filename_prefix, wire::string extension);

			void load();

			void load_animation(bundle::string &json_data);
			void load_shader(bundle::string &json_data);
			void load_sprite(bundle::string &json_data);

			void new_game(bool midnight = true);

			void on_continue();
			void on_down();
			void on_escape();
			void on_exit();
			void on_left();
			void on_reload_shaders();
			void on_right();
			void on_screenshot();
			void on_select();
			void on_up();

		public:
			~Starborn();
			Starborn();

			void run();
	};

	static std::ofstream logger;
	static wire::string logger_cache;
}
