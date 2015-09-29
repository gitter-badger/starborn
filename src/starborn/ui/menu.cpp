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

size_t &ss::ui::Menu::get_position()
{
	return this->position;
}

ss::ui::Menu::Menu()
{
	this->position = 0;
}

ss::vectors::Buttons &ss::ui::Menu::get_buttons()
{
	return this->buttons;
}

void ss::ui::Menu::init(game::Assets &assets, std::function<bundle::string(wire::string)> load_file)
{
	auto texture = this->buttons[this->position].texture;
	auto texture_data = load_file(texture);

	this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
	this->position = 0;

	texture = this->buttons[this->position].selected_texture;
	texture_data = load_file(texture);

	this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
}

void ss::ui::Menu::scroll_down(game::Assets &assets, std::function<bundle::string(wire::string)> load_file)
{
	if(this->buttons.size())
	{
		auto texture = this->buttons[this->position].texture;
		auto texture_data = load_file(texture);

		this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
		this->position = (this->position < (this->buttons.size() - 1)) ? (this->position + 1) : 0;

		texture = this->buttons[this->position].selected_texture;
		texture_data = load_file(texture);

		this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
	}
}

void ss::ui::Menu::scroll_up(game::Assets &assets, std::function<bundle::string(wire::string)> load_file)
{
	if(this->buttons.size())
	{
		auto texture = this->buttons[this->position].texture;
		auto texture_data = load_file(texture);

		this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
		this->position = this->position ? (this->position - 1) : (this->buttons.size() - 1);

		texture = this->buttons[this->position].selected_texture;
		texture_data = load_file(texture);

		this->buttons[this->position].animated_sprite->setTexture(assets.acquire(texture, thor::Resources::fromMemory<sf::Texture>(texture.c_str(), texture.size()), thor::Resources::Reuse));
	}
}
