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

ss::Starborn::~Starborn()
{ $
	this->loading_thread.join();
}

ss::Starborn::Starborn()
{ $
	std::vector<wire::string> critical_files =
	{
		"starborn.exe", "starborn.pdb"
	};

	ss::handle_updated(critical_files);

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

	this->state.switch_state(STATE_LOADING);
	this->loading_thread = std::thread(&Starborn::load, this, critical_files);
}

ss::State &ss::Starborn::get_state()
{ $
	return this->state;
}

void ss::Starborn::load(std::vector<wire::string> &critical_files)
{ $
	if(this->window.isOpen())
	{ $
		std::vector<wire::string> files =
		{
			"assets.zip", "base91.exe", "bundler.exe", "uuid.exe"
		};

		if(ss::update_files(files, critical_files, "https://raw.githubusercontent.com/snailsoft/starborn/" GIT_BRANCH "/patch/", [this, critical_files, files](uint32_t file, wire::string &filename)
		{ $
			this->get_state().set_loading_bar_percent(file, critical_files.size() + files.size());
		}))
			this->on_exit();

		if(this->window.isOpen())
		{ $
			this->state.on_updated();
			
			bundle::archive assets;
			assets.bin(apathy::file("assets.zip").read());

			auto files_loaded = 0;

			for(auto &&asset : assets)
			{ $
				wire::string filename = asset["name"];

				auto animations = filename.matchesi("assets/animations/*.json");
				auto drawables = filename.matchesi("assets/drawables/*.json");
				auto shaders = filename.matchesi("assets/shaders/*.json");

				if(!animations && !drawables && !shaders)
				{ $
					auto data = unpack_asset(asset);

					if(filename.matchesi("assets/shaders/*.frag") || filename.matchesi("assets/shaders/*.vert"))
					{ $
						this->shader_sources[filename] = data;
						std::cout << "Loaded shader: " << filename << std::endl;
					}
					else if(filename.matchesi("assets/textures/*.png"))
					{ $
						auto texture = base91::decode(data);
						this->textures.acquire(filename, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse);
						
						std::cout << "Loaded texture: " << filename << std::endl;
					}
					else if(filename.matchesi("assets/fonts/*.ttf"))
					{ $
						auto font = base91::decode(data);
						this->fonts.acquire(filename, thor::Resources::fromMemory<sf::Font>(font.c_str(), font.size()), thor::Resources::Reuse);
						
						std::cout << "Loaded font: " << filename << std::endl;
					}

					this->state.set_loading_bar_percent(++files_loaded, assets.size());
				}

				if(!this->window.isOpen())
					break;
			}

			if(this->window.isOpen())
			{ $
				for(auto &&asset : assets)
				{ $
					wire::string filename = asset["name"];

					auto animations = filename.matchesi("assets/animations/*.json");
					auto drawables = filename.matchesi("assets/drawables/*.json");
					auto shaders = filename.matchesi("assets/shaders/*.json");

					if(animations || drawables || shaders)
					{ $
						auto data = unpack_asset(asset);

						if(animations)
						{ $
							this->load_animations(data);
							std::cout << "Loaded animations: " << filename << std::endl;
						}
						else if(drawables)
						{ $
							this->load_drawables(data);
							std::cout << "Loaded drawables: " << filename << std::endl;
						}
						else if(shaders)
						{ $
							this->load_shaders(data);
							std::cout << "Loaded shaders: " << filename << std::endl;
						}

						this->state.set_loading_bar_percent(++files_loaded, assets.size());
					}

					if(!this->window.isOpen())
						break;
				}

				if(this->window.isOpen())
				{ $
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
		}
	}
}

void ss::Starborn::load_animations(bundle::string &json_data)
{ $
	Json json(json_data);

	for(auto animation = json.get_document().MemberBegin(); animation != json.get_document().MemberEnd(); ++animation)
	{ $
		wire::string name(animation->name.GetString());

		if(wire::string(animation->value["type"].GetString()) == ANIMATION_TYPE_FADE)
		{ $
			this->animations[name].sprite_animation = thor::FadeAnimation(animation->value["inRatio"].GetDouble(), animation->value["outRatio"].GetDouble());
			this->animations[name].string_animation = thor::FadeAnimation(animation->value["inRatio"].GetDouble(), animation->value["outRatio"].GetDouble());
		}
		else
		{ $
			auto frame_animation = thor::FrameAnimation();
			
			for(auto frame = animation->value["frames"].Begin(); frame != animation->value["frames"].End(); ++frame)
				frame_animation.addFrame((*frame)["ratio"].GetDouble(), sf::IntRect((*frame)["x"].GetInt(), (*frame)["y"].GetInt(), (*frame)["width"].GetInt(), (*frame)["height"].GetInt()));

			this->animations[name].sprite_animation = frame_animation;
		}

		this->animations[name].duration = sf::seconds(animation->value["duration"].GetDouble());
	}
}

void ss::Starborn::load_drawables(bundle::string &json_data)
{ $
	Json json(json_data);

	for(auto drawable = json.get_document().MemberBegin(); drawable != json.get_document().MemberEnd(); ++drawable)
	{ $
		wire::string name(drawable->name.GetString());
		wire::string type(drawable->value["type"].GetString());

		void *new_drawable = nullptr;

		if(type == DRAWABLE_TYPE_ANIMATED_SPRITE)
		{ $
			new_drawable = new AnimatedSprite();

			for(auto animation = drawable->value["animations"].Begin(); animation != drawable->value["animations"].End(); ++animation)
				reinterpret_cast<AnimatedSprite *>(new_drawable)->addAnimation(animation->GetString(), this->animations[animation->GetString()].sprite_animation, this->animations[animation->GetString()].duration);

			if((name == BUTTON_CONTINUE) || (name == BUTTON_EXIT) || (name == BUTTON_LOAD_GAME) || (name == BUTTON_MIDNIGHT) || (name == BUTTON_NEW_GAME) || (name == BUTTON_NIGHTFALL) || (name == BUTTON_OPTIONS))
			{ $
				Button button;

				button.animated_sprite = reinterpret_cast<AnimatedSprite *>(new_drawable);
				button.name = name;
				button.selected_texture = drawable->value["selected_texture"].GetString();
				button.texture = drawable->value["texture"].GetString();

				this->menus[drawable->value["state"].GetString()].get_buttons().push_back(button);
			}
		}
		else if(type == DRAWABLE_TYPE_ANIMATED_STRING)
		{ $
			new_drawable = new AnimatedString();
			auto &string = *reinterpret_cast<AnimatedString *>(new_drawable);

			string.setCharacterSize(drawable->value["size"].GetUint());
			string.setColor(sf::Color::White);
			string.setFont(this->fonts[drawable->value["font"].GetString()]);
			string.setString(wire::string(drawable->value["text"].GetString()).replace("$branch", GIT_BRANCH).replace("$compile_date", __DATE__).replace("$compile_time", __TIME__).replace("$version", STARBORN_VERSION).c_str());
			string.set_position(drawable->value["position"]["anchor"].GetString(), drawable->value["position"]["x"].GetDouble(), drawable->value["position"]["y"].GetDouble());

			for(auto animation = drawable->value["animations"].Begin(); animation != drawable->value["animations"].End(); ++animation)
				string.addAnimation(animation->GetString(), this->animations[animation->GetString()].string_animation, this->animations[animation->GetString()].duration);
		}
		else if(type == DRAWABLE_TYPE_SPRITE)
			new_drawable = new Sprite();

		if(type == DRAWABLE_TYPE_BACKGROUND)
		{ $
			new_drawable = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM)));
			reinterpret_cast<sf::RectangleShape *>(new_drawable)->setFillColor(sf::Color::Transparent);
		}
		else if(type != DRAWABLE_TYPE_ANIMATED_STRING)
		{ $
			auto &sprite = *reinterpret_cast<Sprite *>(new_drawable);

			sprite.has_dynamic_position() = drawable->value["dynamic_position"].GetBool();
			sprite.setTexture(this->textures[drawable->value["texture"].GetString()]);
			sprite.set_position(drawable->value["position"]["anchor"].GetString(), drawable->value["position"]["x"].GetDouble(), drawable->value["position"]["y"].GetDouble(), drawable->value.HasMember("size") ? drawable->value["size"]["x"].GetDouble() : 0.0f, drawable->value.HasMember("size") ? drawable->value["size"]["y"].GetDouble() : 0.0f);
		}

		Drawable drawable_struct;

		drawable_struct.ending_animation = type.starts_with("animated_") ? drawable->value["ending_animation"].GetString() : "";
		drawable_struct.extra_animation = type.starts_with("animated_") ? (drawable->value.HasMember("extra_animation") ? drawable->value["extra_animation"].GetString() : "") : "";
		drawable_struct.drawable = reinterpret_cast<sf::Drawable *>(new_drawable);
		drawable_struct.name = name;
		drawable_struct.render_states.shader = drawable->value.HasMember("shader") ? &this->shaders[drawable->value["shader"].GetString()] : nullptr;
		drawable_struct.reversible = drawable->value.HasMember("reversible") ? drawable->value["reversible"].GetBool() : true;
		drawable_struct.scale = drawable->value.HasMember("scale") ? drawable->value["scale"].GetBool() : false;
		drawable_struct.starting_animation = type.starts_with("animated_") ? drawable->value["starting_animation"].GetString() : "";
		drawable_struct.type = type;

		this->state.get_drawables()[drawable->value["state"].GetString()].push_back(drawable_struct);
	}
}

void ss::Starborn::load_shaders(bundle::string &json_data)
{ $
	Json json(json_data);

	for(auto shader = json.get_document().MemberBegin(); shader != json.get_document().MemberEnd(); ++shader)
	{ $
		wire::string name(shader->name.GetString());

		if(shader->value.HasMember("fragment") && shader->value.HasMember("vertex"))
			this->shaders[name].loadFromMemory(this->shader_sources[shader->value["vertex"].GetString()], this->shader_sources[shader->value["fragment"].GetString()]);

		else if(shader->value.HasMember("fragment"))
			this->shaders[name].loadFromMemory(this->shader_sources[shader->value["fragment"].GetString()], sf::Shader::Fragment);

		else if(shader->value.HasMember("vertex"))
			this->shaders[name].loadFromMemory(this->shader_sources[shader->value["vertex"].GetString()], sf::Shader::Vertex);

		if(shader->value.HasMember("fragment") || shader->value.HasMember("vertex"))
			this->shaders[name].setParameter("texture", sf::Shader::CurrentTexture);
	}
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
	if((this->state.get_state() != STATE_MAIN_MENU) && (this->state.get_state() != STATE_LOADING) && (this->state.get_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_state() != STATE_STARBORN_LOGO))
		this->state.switch_state(STATE_MAIN_MENU, (this->state.get_state() == STATE_RUNNING) ? false : true);

	else
		this->on_exit();
}

void ss::Starborn::on_exit()
{ $
	if(this->state.get_next_state() != STATE_CLOSING)
	{ $
		auto exit = [this]()
		{ $
			std::cout << std::endl;
			std::cout << "on_exit() stack trace:" << std::endl;
			std::cout << std::endl;

			stack_trace();
			this->window.close();
		};

		if((this->state.get_state() == STATE_LOADING) || (this->state.get_state() == STATE_RUNNING) || (this->state.get_state() == STATE_SNAILSOFT_LOGO) || (this->state.get_state() == STATE_STARBORN_LOGO))
			exit();

		else
			this->state.switch_state(STATE_CLOSING, false, exit);
	}
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
	sf::Image(this->window.capture()).saveToFile(get_timestamped_filename("screenshots", "starborn", ".png"));
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

	while(this->window.isOpen())
	{ $
		last_frame_time = last_frame.restart();
		this->state.get_time() += last_frame_time;

		this->actions.update(this->window);
		this->actions.invokeCallbacks(this->callbacks, nullptr);

		if(this->window.isOpen())
		{ $
			this->window.clear();
			this->state.update(last_frame_time, this->window);
			this->window.display();
		}
	}
}
