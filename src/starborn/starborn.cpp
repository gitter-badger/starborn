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
	this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));

	auto attach_logo = [this](constants::State state, std::string name, std::string filename)
	{
		auto *logo = new entities::AnimatedSprite();

		logo->addAnimation("fade", thor::FadeAnimation(0.25f, 0.25f), sf::seconds(4.0f));
		logo->setTexture(this->assets.acquire(name, thor::Resources::fromFile<sf::Texture>(filename), thor::Resources::Reuse));
		logo->setPosition(static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - (logo->getTexture()->getSize().x / 2)), static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - (logo->getTexture()->getSize().y / 2)));

		this->state.attach_drawable(state, ASSET_SNAILSOFT, logo, constants::DRAWABLE_TYPE_ANIMATED_SPRITE);
	};

	attach_logo(constants::STATE_SNAILSOFT_LOGO, ASSET_SNAILSOFT, ASSET_SNAILSOFT_FILENAME);
	attach_logo(constants::STATE_STARBORN_LOGO, ASSET_STARBORN, ASSET_STARBORN_FILENAME);

	this->state.switch_state(constants::STATE_SNAILSOFT_LOGO);
}

void ss::Starborn::on_exit()
{
	this->window.close();
}

void ss::Starborn::run()
{
	sf::Clock last_frame;
	sf::Time last_frame_time;

	while(this->window.isOpen())
	{
		last_frame_time = last_frame.restart();

		this->actions.update(this->window);
		this->actions.invokeCallbacks(this->callbacks, nullptr);

		this->window.clear();
		this->state.update(last_frame_time, this->window);
		this->window.display();
	}
}
