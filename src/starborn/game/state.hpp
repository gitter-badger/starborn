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
			sf::Drawable *drawable;
			sf::RenderStates render_states;

			std::string animation;
			std::string name;
			std::string type;
		};
	};

	namespace vectors
	{
		typedef std::vector<structs::Drawable> Drawables;
	};

	namespace maps
	{
		typedef std::map<std::string, vectors::Drawables> Drawables;
	};

	namespace game
	{
		class State
		{
			private:
				maps::Drawables drawables;
				std::string state;

			public:
				State();

				std::string &get_state();
				vectors::Drawables &get_drawables();

				void attach_drawable(std::string state, std::string name, sf::Drawable *drawable, std::string type, std::string animation = "", sf::Shader *shader = nullptr);
				void switch_state(std::string state);
				void update(sf::Time &last_frame_time, sf::Time &total_time, sf::RenderWindow &window);
		};
	}
}
