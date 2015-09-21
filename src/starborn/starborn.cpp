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

	this->actions[ACTION_DOWN] = thor::Action(sf::Keyboard::Down, thor::Action::PressOnce);
	this->actions[ACTION_EXIT] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
	this->actions[ACTION_RELOAD_SHADERS] = thor::Action(sf::Keyboard::F9, thor::Action::PressOnce);
	this->actions[ACTION_SCREENSHOT] = thor::Action(sf::Keyboard::F5, thor::Action::PressOnce);
	this->actions[ACTION_SELECT] = thor::Action(sf::Keyboard::Return, thor::Action::PressOnce);
	this->actions[ACTION_UP] = thor::Action(sf::Keyboard::Up, thor::Action::PressOnce);

	this->callbacks.connect(ACTION_DOWN, std::bind(&Starborn::on_down, this));
	this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));
	this->callbacks.connect(ACTION_RELOAD_SHADERS, std::bind(&Starborn::on_reload_shaders, this));
	this->callbacks.connect(ACTION_SCREENSHOT, std::bind(&Starborn::on_screenshot, this));
	this->callbacks.connect(ACTION_SELECT, std::bind(&Starborn::on_select, this));
	this->callbacks.connect(ACTION_UP, std::bind(&Starborn::on_up, this));

	this->load_animations();
	this->load_shaders();
	this->load_sprites();

	this->main_menu.init(this->assets);

	this->state.switch_state(STATE_SNAILSOFT_LOGO, [this]()
	{
		this->state.switch_state(STATE_STARBORN_LOGO, [this]()
		{
			this->state.switch_state(STATE_MAIN_MENU, [](){});
		});
	});
}

void ss::Starborn::load_animation(std::string filename)
{
	utilities::Json json(filename);

	for(auto animation = json.get_document().MemberBegin(); animation != json.get_document().MemberEnd(); ++animation)
	{
		if(!strcmp(animation->value["type"].GetString(), ANIMATION_TYPE_FADE))
			this->animations[animation->name.GetString()].animation = thor::FadeAnimation(animation->value["inRatio"].GetDouble(), animation->value["outRatio"].GetDouble());

		else
		{
			auto frame_animation = thor::FrameAnimation();
			
			for(auto frame = animation->value["frames"].Begin(); frame != animation->value["frames"].End(); ++frame)
				frame_animation.addFrame((*frame)["ratio"].GetDouble(), sf::IntRect((*frame)["x"].GetInt(), (*frame)["y"].GetInt(), (*frame)["width"].GetInt(), (*frame)["height"].GetInt()));

			this->animations[animation->name.GetString()].animation = frame_animation;
		}

		this->animations[animation->name.GetString()].duration = sf::seconds(animation->value["duration"].GetDouble());
	}
}

void ss::Starborn::load_animations()
{
	this->load_animation("assets/animations/general.json");
	this->load_animation("assets/animations/ui.json");
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
			this->shaders[shader->name.GetString()].setParameter("pi", static_cast<float>(M_PI));
			this->shaders[shader->name.GetString()].setParameter("texture", sf::Shader::CurrentTexture);
		}
	}
}

void ss::Starborn::load_shaders()
{
	this->load_shader("assets/shaders/shaders.json");
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

			for(auto animation = sprite->value["animations"].Begin(); animation != sprite->value["animations"].End(); ++animation)
				reinterpret_cast<entities::AnimatedSprite *>(new_sprite)->addAnimation(animation->GetString(), this->animations[animation->GetString()].animation, this->animations[animation->GetString()].duration);
		
			if(!strcmp(sprite->name.GetString(), BUTTON_CONTINUE) || !strcmp(sprite->name.GetString(), BUTTON_EXIT) || !strcmp(sprite->name.GetString(), BUTTON_LOAD_GAME) || !strcmp(sprite->name.GetString(), BUTTON_NEW_GAME) || !strcmp(sprite->name.GetString(), BUTTON_OPTIONS))
			{
				structs::Button button;

				button.animated_sprite = reinterpret_cast<entities::AnimatedSprite *>(new_sprite);
				button.name = sprite->name.GetString();
				button.selected_texture = sprite->value["selected_texture"].GetString();
				button.texture = sprite->value["texture"].GetString();

				this->main_menu.get_buttons().push_back(button);
			}
		}
		else if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_DEFAULT))
			new_sprite = new entities::Sprite();

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_BACKGROUND))
		{
			new_sprite = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM)));
			reinterpret_cast<sf::RectangleShape *>(new_sprite)->setFillColor(sf::Color::Transparent);
		}
		else
		{
			auto &final_sprite = *reinterpret_cast<entities::Sprite *>(new_sprite);

			final_sprite.has_dynamic_position() = sprite->value["dynamic_position"].GetBool();
			final_sprite.setTexture(this->assets.acquire(sprite->value["texture"].GetString(), thor::Resources::fromFile<sf::Texture>(sprite->value["texture"].GetString()), thor::Resources::Reuse));
			final_sprite.set_position(sprite->value["position"]["anchor"].GetString(), sprite->value["position"]["x"].GetDouble(), sprite->value["position"]["y"].GetDouble(), sprite->value.HasMember("size") ? sprite->value["size"]["x"].GetDouble() : 0.0f, sprite->value.HasMember("size") ? sprite->value["size"]["y"].GetDouble() : 0.0f);
		}

		auto drawable_type = SPRITE_TYPE_DEFAULT;

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED))
			drawable_type = DRAWABLE_TYPE_ANIMATED_SPRITE;

		else if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_BACKGROUND))
			drawable_type = DRAWABLE_TYPE_BACKGROUND;

		this->state.attach_drawable(sprite->value["state"].GetString(), sprite->name.GetString(), reinterpret_cast<sf::Drawable *>(new_sprite), drawable_type, !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? sprite->value["starting_animation"].GetString() : "", !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? sprite->value["ending_animation"].GetString() : "", sprite->value.HasMember("scale") ? sprite->value["scale"].GetBool() : false, sprite->value.HasMember("shader") ? &this->shaders[sprite->value["shader"].GetString()] : nullptr);
	}
}

void ss::Starborn::load_sprites()
{
	this->load_sprite("assets/sprites/general.json");
	this->load_sprite("assets/sprites/ui.json");
}

void ss::Starborn::on_continue()
{
}

void ss::Starborn::on_down()
{
	if(this->state.get_state() == STATE_MAIN_MENU)
		this->main_menu.scroll_down(this->assets);
}

void ss::Starborn::on_exit()
{
	this->window.close();
}

void ss::Starborn::on_load_game()
{
}

void ss::Starborn::on_new_game()
{
	this->state.switch_state(STATE_NEW_GAME, [this]()
	{
		this->main_menu.init(this->assets);
	});
}

void ss::Starborn::on_options()
{
}

void ss::Starborn::on_reload_shaders()
{
	this->load_shaders();
}

void ss::Starborn::on_screenshot()
{
	sf::Image(this->window.capture()).saveToFile("screenshot.png");
}

void ss::Starborn::on_select()
{
	if(this->state.get_state() == STATE_MAIN_MENU)
	{
		auto &button_name = this->main_menu.get_buttons()[this->main_menu.get_position()].name;

		if(button_name == BUTTON_CONTINUE)
			this->on_continue();

		else if(button_name == BUTTON_EXIT)
			this->on_exit();

		else if(button_name == BUTTON_LOAD_GAME)
			this->on_load_game();

		else if(button_name == BUTTON_NEW_GAME)
			this->on_new_game();

		else if(button_name == BUTTON_OPTIONS)
			this->on_options();
	}
}

void ss::Starborn::on_up()
{
	if(this->state.get_state() == STATE_MAIN_MENU)
		this->main_menu.scroll_up(this->assets);
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
