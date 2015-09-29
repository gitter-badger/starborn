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

namespace ss
{
	namespace structs
	{
		struct Button
		{
			entities::AnimatedSprite *animated_sprite;

			wire::string name;
			wire::string selected_texture;
			wire::string texture;
		};
	}

	namespace vectors
	{
		typedef std::vector<structs::Button> Buttons;
	}

	namespace ui
	{
		class Menu
		{
			private:
				size_t position;
				vectors::Buttons buttons;

			public:
				Menu();

				size_t &get_position();
				vectors::Buttons &get_buttons();

				void init(game::Assets &assets, std::function<bundle::string(wire::string)> load_file);

				void scroll_down(game::Assets &assets, std::function<bundle::string(wire::string)> load_file);
				void scroll_up(game::Assets &assets, std::function<bundle::string(wire::string)> load_file);
		};
	}
}
