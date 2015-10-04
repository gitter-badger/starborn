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
	struct Animation
	{
		sf::Time duration;

		std::function<void (sf::Text &, float)> string_animation;
		std::function<void (Sprite &, float)> sprite_animation;
	};

	struct Button
	{
		AnimatedSprite *animated_sprite;

		wire::string name;
		wire::string selected_texture;
		wire::string texture;
	};

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
}
