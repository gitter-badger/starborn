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

#include <starborn/starborn.hpp>

ss::game::State::State()
{
	this->state = STATE_DEFAULT;
}

ss::vectors::Drawables &ss::game::State::get_drawables()
{
	return this->drawables[this->state];
}

std::string &ss::game::State::get_state()
{
	return this->state;
}

void ss::game::State::attach_drawable(std::string state, std::string name, sf::Drawable *drawable, std::string type, std::string animation)
{
	structs::Drawable new_drawable;

	new_drawable.animation = animation;
	new_drawable.drawable = drawable;
	new_drawable.name = name;
	new_drawable.type = type;

	this->drawables[state].push_back(new_drawable);
}

void ss::game::State::switch_state(std::string state)
{
	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->stopAnimation();
	}

	this->state = state;

	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->playAnimation(drawable.animation);
	}
}

void ss::game::State::update(sf::Time &last_frame_time, sf::RenderWindow &window)
{
	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
		{
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->update(last_frame_time);
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->animate(*reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable));

			if((this->state == STATE_SNAILSOFT_LOGO) && (drawable.name == SPRITE_SNAILSOFT) && !reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->isPlayingAnimation())
				this->switch_state(STATE_STARBORN_LOGO);

			if((this->state == STATE_STARBORN_LOGO) && (drawable.name == SPRITE_STARBORN_VERTICAL) && !reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->isPlayingAnimation())
				this->switch_state(STATE_MAIN_MENU);
		}

		window.draw(*drawable.drawable);
	}
}
