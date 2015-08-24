/*
 * Copyright (C) 2015 Snailsoft <https://github.com/snailsoft/starborn/>
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
		struct Drawable
		{
			constants::DrawableType type;
			sf::Drawable *drawable;
			std::string name;
		};
	};

	namespace vectors
	{
		typedef std::vector<structs::Drawable> Drawables;
	};

	namespace maps
	{
		typedef std::map<constants::State, vectors::Drawables> Drawables;
	};

	namespace entities
	{
		class State
		{
			private:
				constants::State state;
				maps::Drawables drawables;

			public:
				constants::State &get_state();

				State();

				vectors::Drawables &get_drawables();

				void attach_drawable(constants::State state, std::string name, sf::Drawable *drawable, constants::DrawableType type);
				void switch_state(constants::State state);
				void update(sf::Time &last_frame_time, sf::RenderWindow &window);
		};
	}
}
