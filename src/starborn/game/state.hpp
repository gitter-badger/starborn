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
		struct Drawable
		{
			bool reversible;
			bool scale;

			sf::Drawable *drawable;
			sf::RenderStates render_states;

			std::string ending_animation;
			std::string name;
			std::string starting_animation;
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
				bool reverse_animations;
				bool running;
				bool update_state;

				maps::Drawables drawables;

				sf::RenderTexture background;
				sf::Sprite background_sprite;

				std::function<void()> callback;

				std::string next_state;
				std::string state;

				void on_update_animated_sprite(sf::Time &last_frame_time, structs::Drawable &drawable);
				void on_update_background(structs::Drawable &drawable);
				void on_update_sprite(structs::Drawable &drawable);

				void update_shader_parameters(sf::Time &total_time, sf::Shader &shader);

			public:
				bool &is_running();

				maps::Drawables &get_drawables();
				State();
				std::string &get_state();

				void switch_state(std::string state, bool reverse_animations = false, std::function<void()> callback = [](){});
				void update(sf::Time &last_frame_time, sf::Time &total_time, sf::RenderWindow &window);
		};
	}
}
