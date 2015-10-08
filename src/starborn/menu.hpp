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
	class Menu
	{
		private:
			size_t position;
			size_t start;

			std::vector<Button> buttons;

		public:
			Menu();

			size_t &get_position();
			std::vector<Button> &get_buttons();

			void init(thor::ResourceHolder<sf::Texture, wire::string> &textures, size_t position = 0);

			void scroll_down(thor::ResourceHolder<sf::Texture, wire::string> &textures);
			void scroll_up(thor::ResourceHolder<sf::Texture, wire::string> &textures);
	};
}
