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

size_t &ss::ui::Menu::get_position()
{
	return this->position;
}

ss::ui::Menu::Menu()
{
	this->position = 0;
}

ss::vectors::AnimatedSprites &ss::ui::Menu::get_buttons()
{
	return this->buttons;
}

void ss::ui::Menu::scroll_down()
{
	if(this->buttons.size())
	{
		this->buttons[this->position].animated_sprite->playAnimation(ANIMATION_BUTTON);
		this->position = (this->position < (this->buttons.size() - 1)) ? (this->position + 1) : 0;
		this->buttons[this->position].animated_sprite->playAnimation(ANIMATION_BUTTON_SELECTED);
	}
}

void ss::ui::Menu::scroll_up()
{
	if(this->buttons.size())
	{
		this->buttons[this->position].animated_sprite->playAnimation(ANIMATION_BUTTON);
		this->position = this->position ? (this->position - 1) : (this->buttons.size() - 1);
		this->buttons[this->position].animated_sprite->playAnimation(ANIMATION_BUTTON_SELECTED);
	}
}
