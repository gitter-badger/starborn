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
	this->window.setVerticalSyncEnabled(true);

	this->actions[ACTION_EXIT] = thor::Action(sf::Event::Closed) || thor::Action(sf::Keyboard::Escape);
	this->callbacks.connect(ACTION_EXIT, std::bind(&Starborn::on_exit, this));
}

void ss::Starborn::on_exit()
{
	this->window.close();
}

void ss::Starborn::run()
{
	while(this->window.isOpen())
	{
		this->actions.update(this->window);
		this->actions.invokeCallbacks(this->callbacks, nullptr);

		this->window.clear();
		this->window.display();
	}
}
