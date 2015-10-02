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

bool &ss::State::is_running()
{ $
	return this->running;
}

sf::Time &ss::State::get_time()
{ $
	return this->time;
}

ss::State::State()
{ $
	this->background.create(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM));
	this->background.clear(); 

	auto height = 8.0f;
	auto width = static_cast<float>((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM);

	auto x = static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - (width / 2.0f));
	auto y = static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - (height / 2.0f));

	this->loading_bar.addAnimation(ANIMATION_FADE_IN, thor::FadeAnimation(1.0f, 0.0f), sf::seconds(1.0f));
	this->loading_bar.addAnimation(ANIMATION_FADE_OUT, thor::FadeAnimation(0.0f, 1.0f), sf::seconds(1.0f));

	this->loading_bar.setFillColor(sf::Color(0, 192, 192));
	this->loading_bar.setPosition(x, y);
	this->loading_bar.setSize(sf::Vector2f(0.0f, height));

	this->loading_bar_border.addAnimation(ANIMATION_FADE_IN, thor::FadeAnimation(1.0f, 0.0f), sf::seconds(1.0f));
	this->loading_bar_border.addAnimation(ANIMATION_FADE_OUT, thor::FadeAnimation(0.0f, 1.0f), sf::seconds(1.0f));
	
	this->loading_bar_border.setFillColor(sf::Color::Transparent);
	this->loading_bar_border.setOutlineColor(sf::Color(0, 192, 192));
	this->loading_bar_border.setOutlineThickness(1.0f);
	this->loading_bar_border.setPosition(x, y);
	this->loading_bar_border.setSize(sf::Vector2f(width, height));

	Drawable loading_bar;

	loading_bar.drawable = &this->loading_bar;
	//loading_bar.ending_animation = ANIMATION_FADE_OUT;
	loading_bar.name = "loading_bar";
	loading_bar.reversible = true;
	loading_bar.scale = false;
	//loading_bar.starting_animation = ANIMATION_FADE_IN;
	loading_bar.type = DRAWABLE_TYPE_ANIMATED_RECTANGLE;

	this->drawables[STATE_LOADING].push_back(loading_bar);

	loading_bar.drawable = &this->loading_bar_border;
	loading_bar.name = "loading_bar_border";

	this->drawables[STATE_LOADING].push_back(loading_bar);
	
	this->reverse_animations = false;
	this->running = false;
	this->update_state = false;
}

std::map<wire::string, std::vector<ss::Drawable>> &ss::State::get_drawables()
{ $
	return this->drawables;
}

void ss::State::on_update_animated_rectangle(sf::Time &last_frame_time, Drawable &drawable)
{ $
	auto &animated_rectangle = *reinterpret_cast<AnimatedRectangle *>(drawable.drawable);

	animated_rectangle.update(last_frame_time);
	animated_rectangle.animate(animated_rectangle);

	if(drawable.render_states.shader)
		const_cast<sf::Shader *>(drawable.render_states.shader)->setParameter("resolution", static_cast<float>((animated_rectangle.getSize().x * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)), static_cast<float>((animated_rectangle.getSize().y * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)));

	if(this->next_state.length() && animated_rectangle.isPlayingAnimation() && (animated_rectangle.getPlayingAnimation() == drawable.ending_animation))
		this->update_state = false;
}

void ss::State::on_update_animated_sprite(sf::Time &last_frame_time, Drawable &drawable)
{ $
	auto &animated_sprite = *reinterpret_cast<AnimatedSprite *>(drawable.drawable);

	animated_sprite.update(last_frame_time);
	animated_sprite.animate(animated_sprite);

	this->on_update_sprite(drawable);

	if(this->next_state.length() && animated_sprite.isPlayingAnimation() && (animated_sprite.getPlayingAnimation() == drawable.ending_animation))
		this->update_state = false;
}

void ss::State::on_update_background(Drawable &drawable)
{ $
	auto &background = *reinterpret_cast<sf::RectangleShape *>(drawable.drawable);

	if(drawable.render_states.shader)
		const_cast<sf::Shader *>(drawable.render_states.shader)->setParameter("resolution", static_cast<float>((background.getSize().x * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)), static_cast<float>((background.getSize().y * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)));
}

void ss::State::on_update_sprite(Drawable &drawable)
{ $
	auto &sprite = *reinterpret_cast<Sprite *>(drawable.drawable);

	if(drawable.render_states.shader)
		const_cast<sf::Shader *>(drawable.render_states.shader)->setParameter("resolution", static_cast<float>((sprite.getTexture()->getSize().x * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)), static_cast<float>((sprite.getTexture()->getSize().y * SETTING_ZOOM) / (drawable.scale ? SETTING_ZOOM : 1)));
}

void ss::State::on_updated()
{ $
	this->loading_bar.setFillColor(sf::Color(192, 0, 64));
	this->loading_bar_border.setOutlineColor(sf::Color(192, 0, 64));

	this->set_loading_bar_percent(0, 1);
}

void ss::State::set_loading_bar_percent(uint32_t value, uint32_t total)
{ $
	this->loading_bar.setSize(sf::Vector2f(this->loading_bar_border.getSize().x * (static_cast<float>(value) / static_cast<float>(total)), this->loading_bar.getSize().y));
}

void ss::State::switch_state(wire::string state, bool reverse_animations, std::function<void ()> callback)
{ $
	this->callback = callback;
	this->next_state = state;
	this->reverse_animations = reverse_animations;

	if(next_state == STATE_RUNNING)
		this->fade_time = this->time;

	for(auto &&drawable : this->drawables[this->state])
	{ $
		if(((!this->reverse_animations && drawable.ending_animation.length()) || (this->reverse_animations && drawable.starting_animation.length())) || (!drawable.reversible && drawable.ending_animation.length()))
		{ $
			if(drawable.type == DRAWABLE_TYPE_ANIMATED_RECTANGLE)
				reinterpret_cast<AnimatedRectangle *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.starting_animation : drawable.ending_animation) : drawable.ending_animation);

			else if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
				reinterpret_cast<AnimatedSprite *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.starting_animation : drawable.ending_animation) : drawable.ending_animation);
		}
	}
}

void ss::State::update(sf::Time &last_frame_time, sf::RenderWindow &window)
{ $
	if(this->next_state.length())
		this->update_state = true;

	for(auto &&drawable : this->drawables[this->state])
	{ $
		if(drawable.type == DRAWABLE_TYPE_ANIMATED_RECTANGLE)
			this->on_update_animated_rectangle(last_frame_time, drawable);

		else if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
			this->on_update_animated_sprite(last_frame_time, drawable);

		else if(drawable.type == DRAWABLE_TYPE_BACKGROUND)
			this->on_update_background(drawable);

		else if(drawable.type == DRAWABLE_TYPE_SPRITE)
			this->on_update_sprite(drawable);

		if(drawable.render_states.shader)
			this->update_shader_parameters(*const_cast<sf::Shader *>(drawable.render_states.shader));

		if(drawable.scale)
		{ $
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
	{ $
		if((this->state == STATE_LOADING) || (this->state == STATE_RUNNING) || (this->state == STATE_SNAILSOFT_LOGO) || (this->state == STATE_STARBORN_LOGO))
		{ $
			this->fade_time = sf::Time::Zero;
			this->time = sf::Time::Zero;
		}

		this->state = this->next_state;
		this->next_state.clear();

		this->update_state = false;

		for(auto &&drawable : this->drawables[this->state])
		{ $
			if(((!this->reverse_animations && drawable.starting_animation.length()) || (this->reverse_animations && drawable.ending_animation.length())) || (!drawable.reversible && drawable.starting_animation.length()))
			{ $
				if(drawable.type == DRAWABLE_TYPE_ANIMATED_RECTANGLE)
					reinterpret_cast<AnimatedRectangle *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.ending_animation : drawable.starting_animation) : drawable.starting_animation);
		
				else if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
					reinterpret_cast<AnimatedSprite *>(drawable.drawable)->playAnimation(this->reverse_animations ? (drawable.reversible ? drawable.ending_animation : drawable.starting_animation) : drawable.starting_animation);
			}
		}

		if(this->state == STATE_RUNNING)
			this->running = true;
	
		this->callback();
	}
}

void ss::State::update_shader_parameters(sf::Shader &shader)
{ $
	shader.setParameter("fade_time", this->fade_time.asSeconds());
	shader.setParameter("time", this->time.asSeconds());
}

wire::string &ss::State::get_state()
{ $
	return this->state;
}
