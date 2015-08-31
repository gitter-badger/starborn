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
#include <starborn/version.hpp>

ss::Starborn::Starborn()
{
	this->window.create(sf::VideoMode::getDesktopMode(), STARBORN_NAME " " STARBORN_VERSION, sf::Style::None);
	
	this->window.setKeyRepeatEnabled(false);
	this->window.setMouseCursorVisible(false);
	this->window.setVerticalSyncEnabled(true);

	this->view = this->window.getView();

	this->view.zoom(1.0f / SETTING_ZOOM);
	this->view.setCenter(static_cast<float>((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM), static_cast<float>((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM));

	this->window.setView(this->view);

	this->actions[ACTION_EXIT] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Escape);
	this->actions[ACTION_SCREENSHOT] = thor::Action(sf::Keyboard::F5);

	this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));
	this->callbacks.connect(ACTION_SCREENSHOT, std::bind(&Starborn::on_screenshot, this));

	this->load_animations();
	this->load_shaders();
	this->load_sprites();

	this->state.switch_state(STATE_SNAILSOFT_LOGO);
}

void ss::Starborn::load_animation(std::string filename)
{
	utilities::Json json(filename);

	for(auto animation = json.get_document().MemberBegin(); animation != json.get_document().MemberEnd(); ++animation)
	{
		if(!strcmp(animation->value["type"].GetString(), ANIMATION_TYPE_FADE))
			this->animations[animation->name.GetString()].animation = thor::FadeAnimation(animation->value["inRatio"].GetDouble(), animation->value["outRatio"].GetDouble());

		this->animations[animation->name.GetString()].duration = sf::seconds(animation->value["duration"].GetDouble());
	}
}

void ss::Starborn::load_animations()
{
	this->load_animation("assets/animations/general.json");
}

void ss::Starborn::load_shader(std::string filename)
{
	utilities::Json json(filename);

	for(auto shader = json.get_document().MemberBegin(); shader != json.get_document().MemberEnd(); ++shader)
	{
		if(shader->value.HasMember("fragment") && shader->value.HasMember("vertex"))
			this->shaders[shader->name.GetString()].loadFromFile(shader->value["vertex"].GetString(), shader->value["fragment"].GetString());

		else if(shader->value.HasMember("fragment"))
			this->shaders[shader->name.GetString()].loadFromFile(shader->value["fragment"].GetString(), sf::Shader::Fragment);

		else if(shader->value.HasMember("vertex"))
			this->shaders[shader->name.GetString()].loadFromFile(shader->value["vertex"].GetString(), sf::Shader::Vertex);

		if(shader->value.HasMember("fragment") || shader->value.HasMember("vertex"))
		{
			this->shaders[shader->name.GetString()].setParameter("resolution", static_cast<float>(sf::VideoMode::getDesktopMode().width), static_cast<float>(sf::VideoMode::getDesktopMode().height));
			this->shaders[shader->name.GetString()].setParameter("texture", sf::Shader::CurrentTexture);
		}
	}
}

void ss::Starborn::load_shaders()
{
	this->load_shader("assets/shaders/general.json");
}

void ss::Starborn::load_sprite(std::string filename)
{
	utilities::Json json(filename);

	for(auto sprite = json.get_document().MemberBegin(); sprite != json.get_document().MemberEnd(); ++sprite)
	{
		void *new_sprite = nullptr;

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED))
		{
			new_sprite = new entities::AnimatedSprite();
			reinterpret_cast<entities::AnimatedSprite *>(new_sprite)->addAnimation(sprite->value["animation"].GetString(), this->animations[sprite->value["animation"].GetString()].animation, this->animations[sprite->value["animation"].GetString()].duration);
		}
		else if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_DEFAULT))
			new_sprite = new entities::Sprite();

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_NONE))
		{
			new_sprite = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM)));
			reinterpret_cast<sf::RectangleShape *>(new_sprite)->setFillColor(sf::Color::Black);
		}
		else
		{
			reinterpret_cast<entities::Sprite *>(new_sprite)->has_dynamic_position() = sprite->value["dynamic_position"].GetBool();
			reinterpret_cast<entities::Sprite *>(new_sprite)->setTexture(this->assets.acquire(sprite->value["texture"].GetString(), thor::Resources::fromFile<sf::Texture>(sprite->value["texture"].GetString()), thor::Resources::Reuse));
			reinterpret_cast<entities::Sprite *>(new_sprite)->set_position(sprite->value["position"]["anchor"].GetString(), sprite->value["position"]["x"].GetDouble(), sprite->value["position"]["y"].GetDouble());
		}

		this->state.attach_drawable(sprite->value["state"].GetString(), sprite->name.GetString(), reinterpret_cast<sf::Drawable *>(new_sprite), !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? DRAWABLE_TYPE_ANIMATED_SPRITE : DRAWABLE_TYPE_SPRITE, !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? sprite->value["animation"].GetString() : "", sprite->value.HasMember("shader") ? &this->shaders[sprite->value["shader"].GetString()] : nullptr);
	}
}

void ss::Starborn::load_sprites()
{
	this->load_sprite("assets/sprites/general.json");
}

void ss::Starborn::on_exit()
{
	this->window.close();
}

void ss::Starborn::on_screenshot()
{
	sf::Image(this->window.capture()).saveToFile("screenshot.png");
}

void ss::Starborn::run()
{
	sf::Clock last_frame;

	sf::Time last_frame_time;
	sf::Time total_time;

	while(this->window.isOpen())
	{
		last_frame_time = last_frame.restart();
		total_time += last_frame_time;

		this->actions.update(this->window);
		this->actions.invokeCallbacks(this->callbacks, nullptr);

		this->window.clear();
		this->state.update(last_frame_time, total_time, this->window);
		this->window.display();
	}
}
