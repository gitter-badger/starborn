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

sf::Time &ss::State::get_fade_time()
{ $
	return this->fade_time;
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

	this->loading_bar.setFillColor(sf::Color(0, 192, 192));
	this->loading_bar.setPosition(x, y);
	this->loading_bar.setSize(sf::Vector2f(0.0f, height));
	
	this->loading_bar_border.setFillColor(sf::Color::Transparent);
	this->loading_bar_border.setOutlineColor(sf::Color(0, 192, 192));
	this->loading_bar_border.setOutlineThickness(1.0f);
	this->loading_bar_border.setPosition(x, y);
	this->loading_bar_border.setSize(sf::Vector2f(width, height));

	Drawable loading_bar;

	loading_bar.drawable = &this->loading_bar;
	loading_bar.name = "loading_bar";
	loading_bar.scale = false;
	loading_bar.type = DRAWABLE_TYPE_ANIMATED_RECTANGLE;

	this->drawables[STATE_LOADING].push_back(loading_bar);

	loading_bar.drawable = &this->loading_bar_border;
	loading_bar.name = "loading_bar_border";

	this->drawables[STATE_LOADING].push_back(loading_bar);
	
	this->play_animations = false;
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

void ss::State::on_update_animated_string(sf::Time &last_frame_time, Drawable &drawable)
{ $
	auto &animated_string = *reinterpret_cast<AnimatedString *>(drawable.drawable);

	animated_string.update(last_frame_time);
	animated_string.animate(animated_string);

	if(this->next_state.length() && animated_string.isPlayingAnimation() && (animated_string.getPlayingAnimation() == drawable.ending_animation))
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

void ss::State::switch_state(wire::string state, bool play_animations, std::function<void ()> callback)
{ $
	this->callback = callback;
	this->next_state = state;
	this->play_animations = play_animations;
	this->reverse_animations = reverse_animations;

	if((next_state == STATE_CLOSING) || (next_state == STATE_RUNNING))
		this->fade_time = this->time;
	
	for(auto &&drawable : this->drawables[this->state])
	{ $
		if(this->next_state == STATE_CLOSING)
			this->play_animation(drawable, ANIMATION_FADE_OUT);

		else if(this->next_state == STATE_RUNNING)
			this->play_animation(drawable, ANIMATION_FADE_OUT);

		else if(drawable.ending_animation.length() && (drawable.name.starts_with(BUTTON_PREFIX) || this->play_animations))
			this->play_animation(drawable, drawable.ending_animation);
	}
}

void ss::State::play_animation(Drawable &drawable, wire::string animation)
{ $
	if(drawable.type == DRAWABLE_TYPE_ANIMATED_RECTANGLE)
		reinterpret_cast<AnimatedRectangle *>(drawable.drawable)->playAnimation(animation);

	else if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
		reinterpret_cast<AnimatedSprite *>(drawable.drawable)->playAnimation(animation);

	else if(drawable.type == DRAWABLE_TYPE_ANIMATED_STRING)
		reinterpret_cast<AnimatedString *>(drawable.drawable)->playAnimation(animation);

	std::cout << "[" << this->state << (this->next_state.length() ? (" => " + this->next_state) : "") << "] (" << drawable.name << ") Playing animation: " << animation << std::endl;
}

void ss::State::reset_animation(Drawable &drawable)
{ $
	if(drawable.type == DRAWABLE_TYPE_ANIMATED_RECTANGLE)
	{ $
		auto &animated_rectangle = *reinterpret_cast<AnimatedRectangle *>(drawable.drawable);
		auto color = animated_rectangle.getColor();

		color.a = 255;
		animated_rectangle.setColor(color);
	}
	else if(drawable.type == DRAWABLE_TYPE_ANIMATED_SPRITE)
	{ $
		auto &animated_sprite = *reinterpret_cast<AnimatedSprite *>(drawable.drawable);
		auto color = animated_sprite.getColor();

		color.a = 255;
		animated_sprite.setColor(color);
	}
	else if(drawable.type == DRAWABLE_TYPE_ANIMATED_STRING)
	{ $
		auto &animated_string = *reinterpret_cast<AnimatedString *>(drawable.drawable);
		auto color = animated_string.getColor();

		color.a = 255;
		animated_string.setColor(color);
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

		else if(drawable.type == DRAWABLE_TYPE_ANIMATED_STRING)
			this->on_update_animated_string(last_frame_time, drawable);

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
		for(auto &&drawable : this->drawables[this->state])
		{ $
			if(drawable.ending_animation.length() && (drawable.ending_animation == ANIMATION_FADE_OUT))
				this->reset_animation(drawable);
		}

		this->fade_time = sf::Time::Zero;
		this->previous_state = this->state;
		this->state = this->next_state;
		this->next_state.clear();

		this->update_state = false;

		for(auto &&drawable : this->drawables[this->state])
		{ $
			if(drawable.starting_animation.length() && (drawable.name.starts_with(BUTTON_PREFIX) || this->play_animations))
				this->play_animation(drawable, drawable.starting_animation);
		}

		if((this->previous_state == STATE_RUNNING) || (this->state == STATE_SNAILSOFT_LOGO) || (this->state == STATE_STARBORN_LOGO) || (this->state == STATE_RUNNING))
				this->time = sf::Time::Zero;

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

wire::string &ss::State::get_next_state()
{ $
	return this->next_state;
}

wire::string &ss::State::get_previous_state()
{ $
	return this->previous_state;
}

wire::string &ss::State::get_state()
{ $
	return this->state;
}
