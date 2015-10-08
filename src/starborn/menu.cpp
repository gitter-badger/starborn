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

size_t &ss::Menu::get_position()
{ $
	return this->position;
}

ss::Menu::Menu()
{ $
	this->position = 0;
	this->start = 0;
}

std::vector<ss::Button> &ss::Menu::get_buttons()
{ $
	return this->buttons;
}

void ss::Menu::init(thor::ResourceHolder<sf::Texture, wire::string> &textures, size_t position)
{ $
	if(this->buttons.size())
	{ $
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].texture]);
		this->position = position;
		this->start = position;
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].selected_texture]);
	}
}

void ss::Menu::scroll_down(thor::ResourceHolder<sf::Texture, wire::string> &textures)
{ $
	if(this->buttons.size())
	{ $
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].texture]);
		this->position = (this->position < (this->buttons.size() - 1)) ? (this->position + 1) : this->start;
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].selected_texture]);
	}
}

void ss::Menu::scroll_up(thor::ResourceHolder<sf::Texture, wire::string> &textures)
{ $
	if(this->buttons.size())
	{ $
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].texture]);
		this->position = (this->position > this->start) ? (this->position - 1) : (this->buttons.size() - 1);
		this->buttons[this->position].animated_sprite->setTexture(textures[this->buttons[this->position].selected_texture]);
	}
}
