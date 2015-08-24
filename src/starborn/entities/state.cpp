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

ss::constants::State &ss::entities::State::get_state()
{
	return this->state;
}

ss::entities::State::State()
{
	this->state = constants::STATE_SNAILSOFT_LOGO;
}

ss::vectors::Drawables &ss::entities::State::get_drawables()
{
	return this->drawables[this->state];
}

void ss::entities::State::attach_drawable(constants::State state, std::string name, sf::Drawable *drawable, constants::DrawableType type)
{
	structs::Drawable drawable_struct;

	drawable_struct.drawable = drawable;
	drawable_struct.name = name;
	drawable_struct.type = type;

	this->drawables[state].push_back(drawable_struct);
}

void ss::entities::State::switch_state(constants::State state)
{
	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == constants::DRAWABLE_TYPE_ANIMATED_SPRITE)
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->stopAnimation();
	}

	this->state = state;

	for(auto &&drawable : this->drawables[this->state])
	{
		if((drawable.type == constants::DRAWABLE_TYPE_ANIMATED_SPRITE) && ((drawable.name == ASSET_SNAILSOFT) || (drawable.name == ASSET_STARBORN)))
		{
			switch(this->state)
			{
				case constants::STATE_SNAILSOFT_LOGO:
				case constants::STATE_STARBORN_LOGO:
					reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->playAnimation("fade");

					break;

				default:
					break;
			}
		}
	}
}

void ss::entities::State::update(sf::Time &last_frame_time, sf::RenderWindow &window)
{
	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == constants::DRAWABLE_TYPE_ANIMATED_SPRITE)
		{
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->update(last_frame_time);
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->animate(*reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable));

			switch(this->state)
			{
				case constants::STATE_SNAILSOFT_LOGO:
					if((drawable.name == ASSET_SNAILSOFT) && !reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->isPlayingAnimation())
						this->switch_state(constants::STATE_STARBORN_LOGO);

					break;

				case constants::STATE_STARBORN_LOGO:
					if((drawable.name == ASSET_STARBORN) && !reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->isPlayingAnimation())
						this->switch_state(constants::STATE_MAIN_MENU);

					break;

				default:
					break;
			}
		}

		window.draw(*drawable.drawable);
	}
}
