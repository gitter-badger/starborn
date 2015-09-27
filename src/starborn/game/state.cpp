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

bool &ss::game::State::is_running()
{
	return this->running;
}

ss::game::State::State()
{
	this->background.create(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM));
	this->background.clear(); 
	
	this->reverse_animations = false;
	this->running = false;
	this->update_state = false;
}

ss::maps::Drawables &ss::game::State::get_drawables()
{
	return this->drawables;
}

void ss::game::State::on_update_animated_sprite(sf::Time &last_frame_time, structs::Drawable &drawable)
{
	auto &animated_sprite = *reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable);

	animated_sprite.update(last_frame_time);
	animated_sprite.animate(animated_sprite);

	this->on_update_sprite(drawable);

	if(this->next_state.length() && animated_sprite.isPlayingAnimation() && (animated_sprite.getPlayingAnimation() == drawable.ending_animation))
		this->update_state = false;
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

void ss::game::State::switch_state(wire::string state, bool reverse_animations, std::function<void()> callback)
{
	this->callback = callback;
	this->next_state = state;
	this->reverse_animations = reverse_animations;

	for(auto &&drawable : this->drawables[this->state])
	{
		if((drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE) && (((!this->reverse_animations && drawable.ending_animation.length()) || (this->reverse_animations && drawable.starting_animation.length())) || (!drawable.reversible && drawable.ending_animation.length())))
			reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.starting_animation : drawable.ending_animation) : drawable.ending_animation);
	}
}

void ss::game::State::update(sf::Time &last_frame_time, sf::Time &total_time, sf::RenderWindow &window)
{
	if(this->next_state.length())
		this->update_state = true;

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

	if(this->next_state.length() && this->update_state)
	{
		this->state = this->next_state;
		this->next_state.clear();
		this->update_state = false;

		for(auto &&drawable : this->drawables[this->state])
		{
			if((drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE) && (((!this->reverse_animations && drawable.starting_animation.length()) || (this->reverse_animations && drawable.ending_animation.length())) || (!drawable.reversible && drawable.starting_animation.length())))
				reinterpret_cast<entities::AnimatedSprite *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.ending_animation : drawable.starting_animation) : drawable.starting_animation);
		}

		if(this->state == STATE_RUNNING)
			this->running = true;
	
		this->callback();
	}
}

void ss::game::State::update_shader_parameters(sf::Time &total_time, sf::Shader &shader)
{
	shader.setParameter("time", total_time.asSeconds());
}

wire::string &ss::game::State::get_state()
{
	return this->state;
}
