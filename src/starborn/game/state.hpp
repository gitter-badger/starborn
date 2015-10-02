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

			wire::string ending_animation;
			wire::string name;
			wire::string starting_animation;
			wire::string type;
		};
	};

	namespace vectors
	{
		typedef std::vector<structs::Drawable> Drawables;
	};

	namespace maps
	{
		typedef std::map<wire::string, vectors::Drawables> Drawables;
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

				sf::RectangleShape loading_bar;
				sf::RectangleShape loading_bar_border;

				sf::RenderTexture background;
				sf::Sprite background_sprite;
				sf::Time time;

				std::function<void ()> callback;

				wire::string next_state;
				wire::string state;

				void on_update_animated_sprite(sf::Time &last_frame_time, structs::Drawable &drawable);
				void on_update_background(structs::Drawable &drawable);
				void on_update_sprite(structs::Drawable &drawable);

				void update_shader_parameters(sf::Shader &shader);

			public:
				bool &is_running();

				maps::Drawables &get_drawables();
				sf::Time &get_time();

				State();
				wire::string &get_state();

				void on_updated();
				void set_loading_bar_percent(uint32_t value, uint32_t total);
				void switch_state(wire::string state, bool reverse_animations = false, std::function<void ()> callback = [](){});
				void update(sf::Time &last_frame_time, sf::RenderWindow &window);
		};
	}
}
