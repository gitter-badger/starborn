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

#include <starborn/starborn.hpp>

ss::game::State::State()
{
	this->background.create(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM));
	this->background.clear(); 
	
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

void ss::game::State::attach_drawable(std::string state, std::string name, sf::Drawable *drawable, std::string type, std::string animation, bool scale, sf::Shader *shader)
{
	structs::Drawable new_drawable;

	new_drawable.animation = animation;
	new_drawable.drawable = drawable;
	new_drawable.name = name;
	new_drawable.render_states.shader = shader;
	new_drawable.scale = scale;
	new_drawable.type = type;

	this->drawables[state].push_back(new_drawable);
}

void ss::game::State::on_update_animated_sprite(sf::Time &last_frame_time, structs::Drawable &drawable)
{
	auto &animated_sprite = *reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable);

	animated_sprite.update(last_frame_time);
	animated_sprite.animate(animated_sprite);

	if(!animated_sprite.isPlayingAnimation())
	{
		if((this->state == STATE_SNAILSOFT_LOGO) && (drawable.name == SPRITE_SNAILSOFT))
			this->switch_state(STATE_STARBORN_LOGO);

		if((this->state == STATE_STARBORN_LOGO) && (drawable.name == SPRITE_STARBORN_VERTICAL))
			this->switch_state(STATE_MAIN_MENU);
	}

	this->on_update_sprite(drawable);
}

void ss::game::State::on_update_background(structs::Drawable &drawable)
{
	auto &background = *reinterpret_cast<sf::RectangleShape *>(drawable.drawable);

	if(drawable.render_states.shader)
		const_cast<sf::Shader *>(drawable.render_states.shader)->setParameter("resolution", static_cast<float>((background.getSize().x * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)), static_cast<float>((background.getSize().y * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)));
}

void ss::game::State::on_update_sprite(structs::Drawable &drawable)
{
	auto &sprite = *reinterpret_cast<entities::Sprite *>(drawable.drawable);

	if(drawable.render_states.shader)
		const_cast<sf::Shader *>(drawable.render_states.shader)->setParameter("resolution", static_cast<float>((sprite.getTexture()->getSize().x * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)), static_cast<float>((sprite.getTexture()->getSize().y * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)));
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

void ss::game::State::update(sf::Time &last_frame_time, sf::Time &total_time, sf::RenderWindow &window)
{
	for(auto &&drawable : this->drawables[this->state])
	{
		if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
			this->on_update_animated_sprite(last_frame_time, drawable);

		else if(drawable.type == DRAWABLE_TYPE_BACKGROUND)
			this->on_update_background(drawable);

		else if(drawable.type == DRAWABLE_TYPE_SPRITE)
			this->on_update_sprite(drawable);

		if(drawable.render_states.shader)
			this->update_shader_parameters(total_time, *const_cast<sf::Shader *>(drawable.render_states.shader));

		if(drawable.scale)
		{
			this->background.clear();
			this->background.draw(*drawable.drawable, drawable.render_states);
			this->background.display();

			this->background_sprite.setTexture(this->background.getTexture());
			window.draw(this->background_sprite);
		}
		else
			window.draw(*drawable.drawable, drawable.render_states);
	}
}

void ss::game::State::update_shader_parameters(sf::Time &total_time, sf::Shader &shader)
{
	shader.setParameter("time", total_time.asSeconds());
}
