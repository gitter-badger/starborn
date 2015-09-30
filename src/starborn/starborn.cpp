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
#include <git.hpp>
#include <starborn/version.hpp>
#include <windows.h>

bool ss::Starborn::update()
{ $
	wire::string url = "https://github.com/snailsoft/starborn/blob/" GIT_BRANCH "/bin/";

	if(GIT_REVISION_NUMBER < wire::string(flow::download(url + "revision.txt?raw=true").data).as<int32_t>())
	{ $
		this->update_file("assets.zip", url + "assets.zip.sha1?raw=true", url + "assets.zip?raw=true");
		this->update_file("bundler.exe", url + "bundler.exe.sha1?raw=true", url + "bundler.exe?raw=true");

		if(this->update_file("starborn.exe", url + "starborn.exe.sha1?raw=true", url + "starborn.exe?raw=true", false))
		{ $
			PROCESS_INFORMATION process_info;
			STARTUPINFO startup_info;

			memset(&process_info, 0, sizeof(process_info));
			memset(&startup_info, 0, sizeof(startup_info));

			startup_info.cb = sizeof(startup_info);
			CreateProcess(nullptr, "starborn.exe", nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info);

			return true;
		}
	}

	return false;
}

bool ss::Starborn::update_file(wire::string source_filename, wire::string sha1_url, wire::string destination_url, bool delete_old_file)
{ $
	apathy::file file(source_filename);

	if(wire::string(cocoa::SHA1(file.read())) != flow::download(sha1_url).data)
	{ $
		file.patch(flow::download(destination_url).data, delete_old_file);

		return true;
	}

	return false;
}

bundle::string ss::Starborn::unpack_asset(bundle::file &asset)
{ $
	auto data = asset["data"];

	if(bundle::is_packed(data))
		bundle::unpack(data, asset["data"]);

	return data;
}

ss::Starborn::Starborn()
{ $
	this->handle_updated();

	std::cout << STARBORN_NAME << " " << STARBORN_VERSION << std::endl;
	std::cout << "Copyright (C) 2013-2015 " << STARBORN_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
	std::cout << std::endl;
	std::cout << "[" << GIT_BRANCH << "] " << __DATE__ << " " << __TIME__ << std::endl;
	std::cout << std::endl;

	if(!this->update())
	{ $
		this->window.create(sf::VideoMode::getDesktopMode(), STARBORN_NAME " " STARBORN_VERSION, sf::Style::None);

		this->window.setKeyRepeatEnabled(false);
		this->window.setMouseCursorVisible(false);
		this->window.setVerticalSyncEnabled(true);

		this->view = this->window.getView();

		this->view.zoom(1.0f / SETTING_ZOOM);
		this->view.setCenter(static_cast<float>((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM), static_cast<float>((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM));

		this->window.setView(this->view);

		this->actions[ACTION_DOWN] = thor::Action(sf::Keyboard::Down, thor::Action::PressOnce);
		this->actions[ACTION_ESCAPE] = thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce);
		this->actions[ACTION_EXIT] = thor::Action(sf::Event::Closed);
		this->actions[ACTION_LEFT] = thor::Action(sf::Keyboard::Left, thor::Action::PressOnce);
		this->actions[ACTION_RELOAD_SHADERS] = thor::Action(sf::Keyboard::F9, thor::Action::PressOnce);
		this->actions[ACTION_RIGHT] = thor::Action(sf::Keyboard::Right, thor::Action::PressOnce);
		this->actions[ACTION_SCREENSHOT] = thor::Action(sf::Keyboard::F5, thor::Action::PressOnce);
		this->actions[ACTION_SELECT] = thor::Action(sf::Keyboard::Return, thor::Action::PressOnce);
		this->actions[ACTION_UP] = thor::Action(sf::Keyboard::Up, thor::Action::PressOnce);

		this->callbacks.connect(ACTION_DOWN, std::bind(&Starborn::on_down, this));
		this->callbacks.connect(ACTION_ESCAPE, std::bind(&Starborn::on_escape, this));
		this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));
		this->callbacks.connect(ACTION_LEFT, std::bind(&Starborn::on_left, this));
		this->callbacks.connect(ACTION_RELOAD_SHADERS, std::bind(&Starborn::on_reload_shaders, this));
		this->callbacks.connect(ACTION_RIGHT, std::bind(&Starborn::on_right, this));
		this->callbacks.connect(ACTION_SCREENSHOT, std::bind(&Starborn::on_screenshot, this));
		this->callbacks.connect(ACTION_SELECT, std::bind(&Starborn::on_select, this));
		this->callbacks.connect(ACTION_UP, std::bind(&Starborn::on_up, this));

		this->load();

		this->menus[STATE_MAIN_MENU].init(this->textures);
		this->menus[STATE_NEW_GAME].init(this->textures);

		this->state.switch_state(STATE_SNAILSOFT_LOGO, false, [this]()
		{ $
			this->state.switch_state(STATE_STARBORN_LOGO, false, [this]()
			{ $
				this->state.switch_state(STATE_MAIN_MENU);
			});
		});
	}
}

void ss::Starborn::handle_updated()
{ $
	apathy::file starborn_old("starborn.exe.$old");

	if(starborn_old.exists())
	{ $
		while(starborn_old.exists())
			starborn_old.remove();

		bubble::notify("Updated to version " STARBORN_VERSION, STARBORN_NAME);
	}
}

void ss::Starborn::load()
{ $
	bundle::archive assets;
	assets.bin(apathy::file("assets.zip").read());

	for(auto &&asset : assets)
	{ $
		wire::string file = asset["name"];

		auto animation = file.matchesi("assets/animations/*.json");
		auto shader = file.matchesi("assets/shaders/*.json");
		auto sprite = file.matchesi("assets/sprites/*.json");

		if(!animation && !shader && !sprite)
		{ $
			auto data = this->unpack_asset(asset);

			if(file.matchesi("assets/shaders/*.frag") || file.matchesi("assets/shaders/*.vert"))
				this->shader_sources[file] = data;

			else if(file.matchesi("assets/textures/*.png"))
				this->textures.acquire(file, thor::Resources::fromMemory<sf::Texture>(data.c_str(), data.size()), thor::Resources::Reuse);
		}
	}

	for(auto &&asset : assets)
	{ $
		wire::string file = asset["name"];

		auto animation = file.matchesi("assets/animations/*.json");
		auto shader = file.matchesi("assets/shaders/*.json");
		auto sprite = file.matchesi("assets/sprites/*.json");

		if(animation || shader || sprite)
		{ $
			auto data = this->unpack_asset(asset);

			if(animation)
				this->load_animation(data);

			else if(shader)
				this->load_shader(data);

			else if(sprite)
				this->load_sprite(data);
		}
	}
}

void ss::Starborn::load_animation(bundle::string &json_data)
{ $
	utilities::Json json(json_data);

	for(auto animation = json.get_document().MemberBegin(); animation != json.get_document().MemberEnd(); ++animation)
	{ $
		if(!strcmp(animation->value["type"].GetString(), ANIMATION_TYPE_FADE))
			this->animations[animation->name.GetString()].animation = thor::FadeAnimation(animation->value["inRatio"].GetDouble(), animation->value["outRatio"].GetDouble());

		else
		{ $
			auto frame_animation = thor::FrameAnimation();
			
			for(auto frame = animation->value["frames"].Begin(); frame != animation->value["frames"].End(); ++frame)
				frame_animation.addFrame((*frame)["ratio"].GetDouble(), sf::IntRect((*frame)["x"].GetInt(), (*frame)["y"].GetInt(), (*frame)["width"].GetInt(), (*frame)["height"].GetInt()));

			this->animations[animation->name.GetString()].animation = frame_animation;
		}

		this->animations[animation->name.GetString()].duration = sf::seconds(animation->value["duration"].GetDouble());
	}
}

void ss::Starborn::load_shader(bundle::string &json_data)
{ $
	utilities::Json json(json_data);

	for(auto shader = json.get_document().MemberBegin(); shader != json.get_document().MemberEnd(); ++shader)
	{ $
		if(shader->value.HasMember("fragment") && shader->value.HasMember("vertex"))
			this->shaders[shader->name.GetString()].loadFromMemory(this->shader_sources[shader->value["vertex"].GetString()], this->shader_sources[shader->value["fragment"].GetString()]);

		else if(shader->value.HasMember("fragment"))
			this->shaders[shader->name.GetString()].loadFromMemory(this->shader_sources[shader->value["fragment"].GetString()], sf::Shader::Fragment);

		else if(shader->value.HasMember("vertex"))
			this->shaders[shader->name.GetString()].loadFromMemory(this->shader_sources[shader->value["vertex"].GetString()], sf::Shader::Vertex);

		if(shader->value.HasMember("fragment") || shader->value.HasMember("vertex"))
			this->shaders[shader->name.GetString()].setParameter("texture", sf::Shader::CurrentTexture);
	}
}

void ss::Starborn::load_sprite(bundle::string &json_data)
{ $
	utilities::Json json(json_data);

	for(auto sprite = json.get_document().MemberBegin(); sprite != json.get_document().MemberEnd(); ++sprite)
	{ $
		void *new_sprite = nullptr;

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED))
		{ $
			new_sprite = new entities::AnimatedSprite();

			for(auto animation = sprite->value["animations"].Begin(); animation != sprite->value["animations"].End(); ++animation)
				reinterpret_cast<entities::AnimatedSprite *>(new_sprite)->addAnimation(animation->GetString(), this->animations[animation->GetString()].animation, this->animations[animation->GetString()].duration);
		
			if(!strcmp(sprite->name.GetString(), BUTTON_CONTINUE) || !strcmp(sprite->name.GetString(), BUTTON_EXIT) || !strcmp(sprite->name.GetString(), BUTTON_LOAD_GAME) ||
				!strcmp(sprite->name.GetString(), BUTTON_MIDNIGHT) || !strcmp(sprite->name.GetString(), BUTTON_NEW_GAME) || !strcmp(sprite->name.GetString(), BUTTON_NIGHTFALL) ||
				!strcmp(sprite->name.GetString(), BUTTON_OPTIONS))
			{ $
				structs::Button button;

				button.animated_sprite = reinterpret_cast<entities::AnimatedSprite *>(new_sprite);
				button.name = sprite->name.GetString();
				button.selected_texture = sprite->value["selected_texture"].GetString();
				button.texture = sprite->value["texture"].GetString();

				this->menus[sprite->value["state"].GetString()].get_buttons().push_back(button);
			}
		}
		else if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_DEFAULT))
			new_sprite = new entities::Sprite();

		if(!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_BACKGROUND))
		{ $
			new_sprite = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM)));
			reinterpret_cast<sf::RectangleShape *>(new_sprite)->setFillColor(sf::Color::Transparent);
		}
		else
		{ $
			auto &final_sprite = *reinterpret_cast<entities::Sprite *>(new_sprite);

			final_sprite.has_dynamic_position() = sprite->value["dynamic_position"].GetBool();
			final_sprite.setTexture(this->textures[sprite->value["texture"].GetString()]);
			final_sprite.set_position(sprite->value["position"]["anchor"].GetString(), sprite->value["position"]["x"].GetDouble(), sprite->value["position"]["y"].GetDouble(), sprite->value.HasMember("size") ? sprite->value["size"]["x"].GetDouble() : 0.0f, sprite->value.HasMember("size") ? sprite->value["size"]["y"].GetDouble() : 0.0f);
		}

		structs::Drawable drawable;

		drawable.ending_animation = !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? sprite->value["ending_animation"].GetString() : "";
		drawable.drawable = reinterpret_cast<sf::Drawable *>(new_sprite);
		drawable.name = sprite->name.GetString();
		drawable.render_states.shader = sprite->value.HasMember("shader") ? &this->shaders[sprite->value["shader"].GetString()] : nullptr;
		drawable.reversible = sprite->value.HasMember("reversible") ? sprite->value["reversible"].GetBool() : true;
		drawable.scale = sprite->value.HasMember("scale") ? sprite->value["scale"].GetBool() : false;
		drawable.starting_animation = !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? sprite->value["starting_animation"].GetString() : "";
		drawable.type = !strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_ANIMATED) ? DRAWABLE_TYPE_ANIMATED_SPRITE : (!strcmp(sprite->value["type"].GetString(), SPRITE_TYPE_BACKGROUND) ? DRAWABLE_TYPE_BACKGROUND : SPRITE_TYPE_DEFAULT);

		this->state.get_drawables()[sprite->value["state"].GetString()].push_back(drawable);
	}
}

void ss::Starborn::log(bool open, bool feed, bool close, const std::string &line)
{ $
	if(open || close)
	{ $
		if(open)
			logger.open(get_filename("logs", "starborn", ".txt").c_str(), std::ios::app | std::ios::binary | std::ios::out);
		
		logger << line << std::endl;

		if(close)
			logger.close();
	}
	else if(feed)
	{ $
		logger << sand::format(sand::now(), "[mm/dd/yyyy HH:MM:SS] ") << logger_cache << "\r\n";
		logger_cache.clear();
	}
	else
		logger_cache += line;
}

void ss::Starborn::new_game(bool midnight)
{ $
	this->state.switch_state(STATE_RUNNING);
}

void ss::Starborn::on_continue()
{ $
	if(this->state.is_running())
		this->state.switch_state(STATE_RUNNING);
}

void ss::Starborn::on_down()
{ $
	if(this->state.get_state() == STATE_MAIN_MENU)
		this->menus[this->state.get_state()].scroll_down(this->textures);
}

void ss::Starborn::on_escape()
{ $
	if((this->state.get_state() != STATE_MAIN_MENU) && (this->state.get_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_state() != STATE_STARBORN_LOGO))
		this->state.switch_state(STATE_MAIN_MENU, (this->state.get_state() == STATE_RUNNING) ? false : true);

	else
		this->window.close();
}

void ss::Starborn::on_exit()
{ $
	this->window.close();
}

void ss::Starborn::on_left()
{ $
	if(this->state.get_state() == STATE_NEW_GAME)
		this->menus[this->state.get_state()].scroll_up(this->textures);
}

void ss::Starborn::on_reload_shaders()
{ $
}

void ss::Starborn::on_right()
{ $
	if(this->state.get_state() == STATE_NEW_GAME)
		this->menus[this->state.get_state()].scroll_down(this->textures);
}

void ss::Starborn::on_screenshot()
{ $
	sf::Image(this->window.capture()).saveToFile(get_filename("screenshots", "starborn", ".png"));
}

void ss::Starborn::on_select()
{ $
	if((this->state.get_state() != STATE_RUNNING) && (this->state.get_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_state() != STATE_STARBORN_LOGO))
	{ $
		auto &menu = this->menus[this->state.get_state()];
		auto &button_name = menu.get_buttons()[menu.get_position()].name;
		
		if(button_name == BUTTON_CONTINUE)
			this->on_continue();

		else if(button_name == BUTTON_EXIT)
			this->on_exit();

		else if(button_name == BUTTON_MIDNIGHT)
			this->new_game();

		else if(button_name == BUTTON_NEW_GAME)
			this->state.switch_state(STATE_NEW_GAME);

		else if(button_name == BUTTON_NIGHTFALL)
			this->new_game(false);
	}
}

void ss::Starborn::on_up()
{ $
	if(this->state.get_state() == STATE_MAIN_MENU)
		this->menus[this->state.get_state()].scroll_up(this->textures);
}

void ss::Starborn::run()
{ $
	sf::Clock last_frame;

	sf::Time last_frame_time;
	sf::Time total_time;

	while(this->window.isOpen())
	{ $
		last_frame_time = last_frame.restart();
		total_time += last_frame_time;

		this->actions.update(this->window);
		this->actions.invokeCallbacks(this->callbacks, nullptr);

		this->window.clear();
		this->state.update(last_frame_time, total_time, this->window);
		this->window.display();
	}
}

wire::string ss::Starborn::get_filename(wire::string directory, wire::string filename_prefix, wire::string extension)
{ $
	apathy::path path(directory);

	if(!path.exists())
		apathy::path::md(path);

	return directory + "/" + filename_prefix + "-" + sand::format(sand::now(), "mm-dd-yy-HH-MM-SS") + extension;
}
