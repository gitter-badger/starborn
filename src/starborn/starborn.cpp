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

sf::Music &ss::Starborn::get_title_music()
{ $
	return this->title_music;
}

sf::Music &ss::Starborn::get_world_music()
{ $
	return this->world_music;
}

ss::Starborn::~Starborn()
{ $
	this->loading_thread.join();

	this->title_music.stop();
	this->world_music.stop();

	ss::log_cpu_usage();
}

ss::Starborn::Starborn()
{ $
	std::vector<wire::string> critical_files =
	{
		"openal32.dll", "starborn.exe", "starborn.pdb"
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
	this->actions[ACTION_RIGHT] = thor::Action(sf::Keyboard::Right, thor::Action::PressOnce);
	this->actions[ACTION_SCREENSHOT] = thor::Action(sf::Keyboard::F5, thor::Action::PressOnce);
	this->actions[ACTION_SELECT] = thor::Action(sf::Keyboard::Return, thor::Action::PressOnce);
	this->actions[ACTION_UP] = thor::Action(sf::Keyboard::Up, thor::Action::PressOnce);

	this->callbacks.connect(ACTION_DOWN, std::bind(&Starborn::on_down, this));
	this->callbacks.connect(ACTION_ESCAPE, std::bind(&Starborn::on_escape, this));
	this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));
	this->callbacks.connect(ACTION_LEFT, std::bind(&Starborn::on_left, this));
	this->callbacks.connect(ACTION_RIGHT, std::bind(&Starborn::on_right, this));
	this->callbacks.connect(ACTION_SCREENSHOT, std::bind(&Starborn::on_screenshot, this));
	this->callbacks.connect(ACTION_SELECT, std::bind(&Starborn::on_select, this));
	this->callbacks.connect(ACTION_UP, std::bind(&Starborn::on_up, this));

	this->music_volume = 100.0f;
	this->sound_volume = 100.0f;

	this->title_music.setLoop(true);
	this->world_music.setLoop(true);

	this->state.switch_state(STATE_LOADING);
	this->loading_thread = std::thread(&Starborn::load, this, critical_files);
}

ss::State &ss::Starborn::get_state()
{ $
	return this->state;
}

void ss::Starborn::clean_sounds()
{ $
	for(auto i = 0; i < this->sounds.size(); ++i)
	{ $
		if(this->sounds[i]->getStatus() == sf::Sound::Stopped)
		{ $
			delete this->sounds[i];
			this->sounds.erase(this->sounds.begin() + i);
		}
	}
}

void ss::Starborn::fade_sounds(bool music)
{ $
	if(music)
	{ $
		if((this->state.get_previous_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_fade_time().asSeconds()))
		{ $
			this->title_music.setVolume(std::max(0.0f, this->music_volume * (1.0f - (this->state.get_time() - this->state.get_fade_time()).asSeconds())));
			this->world_music.setVolume(std::max(0.0f, this->music_volume * (1.0f - (this->state.get_time() - this->state.get_fade_time()).asSeconds())));

			std::cout << this->state.get_fade_time().asSeconds() << " fade out: " << this->music_volume * (1.0f - (this->state.get_time() - this->state.get_fade_time()).asSeconds()) << std::endl;
		}
		else if((this->state.get_previous_state() != STATE_STARBORN_LOGO) && (this->state.get_time() < this->animations[ANIMATION_FADE_IN].duration))
		{ $
			this->title_music.setVolume(std::min(100.0f, this->music_volume * this->state.get_time().asSeconds()));
			this->world_music.setVolume(std::min(100.0f, this->music_volume * this->state.get_time().asSeconds()));

			std::cout << "fade in: " << this->music_volume * this->state.get_time().asSeconds() << std::endl;
		}
		else if(this->title_music.getVolume() != this->music_volume)
			this->title_music.setVolume(this->music_volume);

		else if(this->world_music.getVolume() != this->music_volume)
			this->world_music.setVolume(this->music_volume);
	}
	else
	{ $
		for(auto &&sound : this->sounds)
		{ $
			if((this->state.get_state() == STATE_SNAILSOFT_LOGO) && (this->state.get_time() >= (this->animations[ANIMATION_FADE].duration - this->animations[ANIMATION_FADE_OUT].duration)))
				sound->setVolume(std::max(0.0f, this->music_volume * (1.0f - (this->state.get_time() - (this->animations[ANIMATION_FADE].duration - this->animations[ANIMATION_FADE_OUT].duration)).asSeconds())));

			else if(this->state.get_fade_time().asSeconds())
				sound->setVolume(std::max(0.0f, this->sound_volume * (1.0f - (this->state.get_time() - this->state.get_fade_time()).asSeconds())));

			else if(this->state.get_time() < this->animations[ANIMATION_FADE_IN].duration)
				sound->setVolume(std::max(0.0f, this->sound_volume * this->state.get_time().asSeconds()));

			else if(sound->getVolume() != this->sound_volume)
				sound->setVolume(this->sound_volume);
		}
	}
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

					if(filename.matchesi("assets/audio/music/*.flac"))
					{ $
						this->raw_music[filename] = base91::decode(data);
						std::cout << "Loaded music: " << filename << std::endl;
					}
					else if(filename.matchesi("assets/audio/sounds/*.flac"))
					{ $
						try
						{ $
							auto sound = base91::decode(data);
							this->raw_sounds.acquire(filename, thor::Resources::fromMemory<sf::SoundBuffer>(sound.c_str(), sound.size()), thor::Resources::Reuse);
							
							std::cout << "Loaded sound: " << filename << std::endl;
						}
						catch(thor::ResourceLoadingException& exception)
						{ $
							std::cout << "Error loading sound: " << filename << ": " << exception.what() << std::endl;
						}						
					}
					else if(filename.matchesi("assets/fonts/*.ttf"))
					{ $
						try
						{ $
							this->fonts[filename].first = base91::decode(data);
							this->fonts[filename].second.acquire(filename, thor::Resources::fromMemory<sf::Font>(this->fonts[filename].first.c_str(), this->fonts[filename].first.size()), thor::Resources::Reuse);
							
							std::cout << "Loaded font: " << filename << std::endl;
						}
						catch(thor::ResourceLoadingException& exception)
						{ $
							std::cout << "Error loading font: " << filename << ": " << exception.what() << std::endl;
						}
					}
					else if(filename.matchesi("assets/shaders/*.frag") || filename.matchesi("assets/shaders/*.vert"))
					{ $
						this->shader_sources[filename] = data;
						std::cout << "Loaded shader: " << filename << std::endl;
					}
					else if(filename.matchesi("assets/textures/*.png"))
					{ $
						try
						{ $
							auto texture = base91::decode(data);
							this->textures.acquire(filename, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse);
							
							std::cout << "Loaded texture: " << filename << std::endl;
						}
						catch(thor::ResourceLoadingException& exception)
						{ $
							std::cout << "Error loading texture: " << filename << ": " << exception.what() << std::endl;
						}
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
					auto shaders = filename.matchesi("assets/shaders/*.json");

					if(animations || shaders)
					{ $
						auto data = unpack_asset(asset);

						if(animations)
						{ $
							this->load_animations(data);
							std::cout << "Loaded animations: " << filename << std::endl;
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
					for(auto &&asset : assets)
					{ $
						wire::string filename = asset["name"];

						if(filename.matchesi("assets/drawables/*.json"))
						{ $
							auto data = unpack_asset(asset);
							this->load_drawables(unpack_asset(asset));

							std::cout << "Loaded drawables: " << filename << std::endl;
							this->state.set_loading_bar_percent(++files_loaded, assets.size());
						}

						if(!this->window.isOpen())
							break;
					}

					if(this->window.isOpen())
					{ $
						this->state.switch_state(STATE_SNAILSOFT_LOGO, true, [this]()
						{ $
							this->menus[STATE_MAIN_MENU].init(this->textures, 1);
							this->menus[STATE_NEW_GAME].init(this->textures);

							this->play_sound(SOUND_SNAILSOFT);
							this->state.switch_state(STATE_STARBORN_LOGO, true, [this]()
							{ $
								this->play_sound(MUSIC_TITLE, true);
								this->state.switch_state(STATE_MAIN_MENU);
							});
						});
					}
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
		auto &states = drawable->value["states"];

		wire::string name(drawable->name.GetString());
		wire::string type(drawable->value["type"].GetString());

		for(auto i = 0u; i < states.Size(); ++i)
		{ $
			void *new_drawable = nullptr;

			if(type == DRAWABLE_TYPE_ANIMATED_SPRITE)
			{ $
				new_drawable = new AnimatedSprite();
				auto &animated_sprite = *reinterpret_cast<AnimatedSprite *>(new_drawable);

				for(auto animation = drawable->value["animations"].Begin(); animation != drawable->value["animations"].End(); ++animation)
					animated_sprite.addAnimation(animation->GetString(), this->animations[animation->GetString()].sprite_animation, this->animations[animation->GetString()].duration);

				if(name.starts_with(BUTTON_PREFIX))
				{ $
					Button button;

					button.animated_sprite = &animated_sprite;
					button.name = name;
					button.selected_texture = drawable->value["selected_texture"].GetString();
					button.texture = drawable->value["texture"].GetString();

					this->menus[states[i].GetString()].get_buttons().push_back(button);
				}
			}
			else if(type == DRAWABLE_TYPE_ANIMATED_STRING)
			{ $
				new_drawable = new AnimatedString();
				auto &animated_string = *reinterpret_cast<AnimatedString *>(new_drawable);

				animated_string.setCharacterSize(drawable->value["size"].GetUint());
				animated_string.setColor(sf::Color::White);
				animated_string.setFont(this->fonts[drawable->value["font"].GetString()].second[drawable->value["font"].GetString()]);
				animated_string.setString(wire::string(drawable->value["text"].GetString()).replace("$branch", GIT_BRANCH).replace("$compile_date", __DATE__).replace("$compile_time", __TIME__).replace("$version", STARBORN_VERSION).c_str());
				animated_string.set_position(drawable->value["position"]["anchor"].GetString(), drawable->value["position"]["x"].GetDouble(), drawable->value["position"]["y"].GetDouble());

				for(auto animation = drawable->value["animations"].Begin(); animation != drawable->value["animations"].End(); ++animation)
					animated_string.addAnimation(animation->GetString(), this->animations[animation->GetString()].string_animation, this->animations[animation->GetString()].duration);
			}
			else if(type == DRAWABLE_TYPE_BACKGROUND)
			{ $
				new_drawable = new sf::RectangleShape(sf::Vector2f(static_cast<float>(sf::VideoMode::getDesktopMode().width / SETTING_ZOOM), static_cast<float>(sf::VideoMode::getDesktopMode().height / SETTING_ZOOM)));
				reinterpret_cast<sf::RectangleShape *>(new_drawable)->setFillColor(sf::Color::Transparent);
			}
			else if(type == DRAWABLE_TYPE_SPRITE)
				new_drawable = new Sprite();

			if((type == DRAWABLE_TYPE_ANIMATED_SPRITE) || (type == DRAWABLE_TYPE_SPRITE))
			{ $
				auto &sprite = *reinterpret_cast<Sprite *>(new_drawable);

				sprite.has_dynamic_position() = drawable->value["dynamic_position"].GetBool();
				sprite.setTexture(this->textures[drawable->value["texture"].GetString()]);
				sprite.set_position(drawable->value["position"]["anchor"].GetString(), drawable->value["position"]["x"].GetDouble(), drawable->value["position"]["y"].GetDouble(), drawable->value.HasMember("size") ? drawable->value["size"]["x"].GetDouble() : 0.0f, drawable->value.HasMember("size") ? drawable->value["size"]["y"].GetDouble() : 0.0f);
			}

			Drawable drawable_struct;

			drawable_struct.ending_animation = type.starts_with(ANIMATED_DRAWABLE_PREFIX) ? ((i == (states.Size() - 1)) ? drawable->value["ending_animation"].GetString() : "") : "";
			drawable_struct.drawable = reinterpret_cast<sf::Drawable *>(new_drawable);
			drawable_struct.name = name;
			drawable_struct.render_states.shader = drawable->value.HasMember("shader") ? &this->shaders[drawable->value["shader"].GetString()] : nullptr;
			drawable_struct.scale = drawable->value.HasMember("scale") ? drawable->value["scale"].GetBool() : false;
			drawable_struct.starting_animation = type.starts_with(ANIMATED_DRAWABLE_PREFIX) ? (!i ? drawable->value["starting_animation"].GetString() : "") : "";
			drawable_struct.type = type;

			this->state.get_drawables()[states[i].GetString()].push_back(drawable_struct);
		}
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
	this->state.switch_state(STATE_RUNNING, true, [this]()
	{ $
		this->get_title_music().pause();
		this->play_sound(MUSIC_CRYPT, true);
	});
}

void ss::Starborn::on_continue()
{ $
	if(this->state.is_running())
	{ $
		this->state.switch_state(STATE_RUNNING, true, [this]()
		{ $
			this->get_title_music().pause();
			this->get_world_music().play();
		});
	}
}

void ss::Starborn::on_down()
{ $
	if(this->state.get_state() == STATE_MAIN_MENU)
	{ $
		this->play_sound(SOUND_MENU_SCROLL);
		this->menus[this->state.get_state()].scroll_down(this->textures);
	}
}

void ss::Starborn::on_escape()
{ $
	if((this->state.get_state() != STATE_MAIN_MENU) && (this->state.get_state() != STATE_LOADING) && (this->state.get_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_state() != STATE_STARBORN_LOGO))
	{ $
		if(!this->state.get_next_state().length())
		{ $
			if(this->state.get_state() == STATE_RUNNING)
			{ $
				this->menus[STATE_MAIN_MENU].init(this->textures);
				this->menus[STATE_NEW_GAME].init(this->textures);
			}
			else
				this->play_sound(SOUND_MENU_SELECT);

			this->state.switch_state(STATE_MAIN_MENU, (this->state.get_state() == STATE_RUNNING) ? true : false, [this]()
			{ $
				if(this->get_state().get_previous_state() == STATE_RUNNING)
				{ $
					this->get_world_music().pause();
					this->play_sound(MUSIC_TITLE, true);
				}
			});
		}
	}
	else if(!this->state.get_next_state().length() || (this->state.get_state() == STATE_LOADING) || (this->state.get_state() == STATE_SNAILSOFT_LOGO) || (this->state.get_state() == STATE_STARBORN_LOGO))
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
			this->state.switch_state(STATE_CLOSING, true, exit);
	}
}

void ss::Starborn::on_left()
{ $
	if(this->state.get_state() == STATE_NEW_GAME)
	{ $
		this->play_sound(SOUND_MENU_SCROLL);
		this->menus[this->state.get_state()].scroll_up(this->textures);
	}
}

void ss::Starborn::on_right()
{ $
	if(this->state.get_state() == STATE_NEW_GAME)
	{ $
		this->play_sound(SOUND_MENU_SCROLL);
		this->menus[this->state.get_state()].scroll_down(this->textures);
	}
}

void ss::Starborn::on_screenshot()
{ $
	sf::Image(this->window.capture()).saveToFile(get_timestamped_filename("screenshots", "starborn", ".png"));
}

void ss::Starborn::on_select()
{ $
	if(!this->state.get_next_state().length() && (this->state.get_state() != STATE_RUNNING) && (this->state.get_state() != STATE_SNAILSOFT_LOGO) && (this->state.get_state() != STATE_STARBORN_LOGO))
	{ $
		this->play_sound(SOUND_MENU_SELECT);

		auto &menu = this->menus[this->state.get_state()];
		auto &button_name = menu.get_buttons()[menu.get_position()].name;
		
		if(button_name == (BUTTON_PREFIX "continue"))
			this->on_continue();

		else if(button_name == (BUTTON_PREFIX "exit"))
			this->on_exit();

		else if(button_name == (BUTTON_PREFIX "midnight"))
			this->new_game();

		else if(button_name == (BUTTON_PREFIX "new_game"))
			this->state.switch_state(STATE_NEW_GAME);

		else if(button_name == (BUTTON_PREFIX "nightfall"))
			this->new_game(false);
	}
}

void ss::Starborn::on_up()
{ $
	if(this->state.get_state() == STATE_MAIN_MENU)
	{ $
		this->play_sound(SOUND_MENU_SCROLL);
		this->menus[this->state.get_state()].scroll_up(this->textures);
	}
}

void ss::Starborn::play_sound(wire::string filename, bool music)
{ $
	this->clean_sounds();

	if(music)
	{ $
		if(this->state.get_state() == STATE_RUNNING)
		{ $
			this->world_music.stop();
			this->world_music.openFromMemory(this->raw_music[filename].c_str(), this->raw_music[filename].size());
			this->world_music.play();
		}
		else
		{ $
			this->title_music.stop();
			this->title_music.openFromMemory(this->raw_music[filename].c_str(), this->raw_music[filename].size());
			this->title_music.play();
		}

		this->fade_sounds(true);
	}
	else
	{ $
		auto sound = new sf::Sound();

		sound->setBuffer(this->raw_sounds[filename]);
		sound->play();

		this->sounds.push_back(sound);
		this->fade_sounds();
	}

	std::cout << "[" << this->state.get_state() << (this->state.get_next_state().length() ? (" => " + this->state.get_next_state()) : "") << "] Playing audio: " << filename << std::endl;

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

		this->fade_sounds();
		this->fade_sounds(true);

		if(this->window.isOpen())
		{ $
			this->window.clear();
			this->state.update(last_frame_time, this->window);
			this->window.display();
		}
	}
}
