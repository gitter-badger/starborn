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

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

#include <SFML/Graphics.hpp>

#include <Thor/Animations.hpp>
#include <Thor/Input.hpp>
#include <Thor/Resources.hpp>

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

#include <starborn/game/state.hpp>
#include <starborn/game/typedefs.hpp>

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
		typedef std::map<std::string, sf::Shader> Shaders;
		typedef std::map<std::string, structs::Animation> Animations;
	};

	class Starborn
	{
		private:
			game::Assets assets;
			game::State state;

			maps::Animations animations;
			maps::Shaders shaders;

			sf::RenderWindow window;
			sf::View view;

			thor::ActionMap<std::string> actions;
			thor::ActionMap<std::string>::CallbackSystem callbacks;

			ui::Menu main_menu;

			void load();

			void load_animation(std::string filename);
			void load_animations();

			void load_shader(std::string filename);
			void load_shaders();

			void load_sprite(std::string filename);
			void load_sprites();

			void on_continue();
			void on_down();
			void on_exit();
			void on_load_game();
			void on_new_game();
			void on_options();
			void on_reload_shaders();
			void on_screenshot();
			void on_select();
			void on_up();

		public:
			Starborn();

			void run();
	};
}
