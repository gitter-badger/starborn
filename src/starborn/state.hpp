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
	class State
	{
		private:
			AnimatedRectangle loading_bar;
			AnimatedRectangle loading_bar_border;

			bool reverse_animations;
			bool running;
			bool update_state;

			sf::RenderTexture background;
			sf::Sprite background_sprite;

			sf::Time fade_time;
			sf::Time time;

			std::function<void ()> callback;
			std::map<wire::string, std::vector<Drawable>> drawables;

			wire::string next_state;
			wire::string state;

			void on_update_animated_rectangle(sf::Time &last_frame_time, Drawable &drawable);
			void on_update_animated_sprite(sf::Time &last_frame_time, Drawable &drawable);
			void on_update_animated_string(sf::Time &last_frame_time, Drawable &drawable);
			void on_update_background(Drawable &drawable);
			void on_update_sprite(Drawable &drawable);

			void update_shader_parameters(sf::Shader &shader);

		public:
			bool &is_running();

			sf::Time &get_time();
			State();

			std::map<wire::string, std::vector<Drawable>> &get_drawables();

			wire::string &get_next_state();
			wire::string &get_state();

			void on_updated();
			void set_loading_bar_percent(uint32_t value, uint32_t total);
			void switch_state(wire::string state, bool reverse_animations = false, std::function<void ()> callback = [](){});
			void update(sf::Time &last_frame_time, sf::RenderWindow &window);
	};
}
