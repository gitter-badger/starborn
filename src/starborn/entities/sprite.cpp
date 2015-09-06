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

bool &ss::entities::Sprite::has_dynamic_position()
{
	return this->dynamic_position;
}

ss::entities::Sprite::Sprite()
{
	this->dynamic_position = false;
}

void ss::entities::Sprite::set_position(std::string anchor, float x, float y, float width, float height)
{
	auto new_x = 0.0f;
	auto new_y = 0.0f;
	
	if(anchor == ANCHOR_BOTTOM)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>((sf::VideoMode::getDesktopMode().height / SETTING_ZOOM) - (!height ? this->getTexture()->getSize().y : height)) + y;
	}
	else if(anchor == ANCHOR_BOTTOM_LEFT)
	{
		new_x += x;
		new_y += static_cast<float>((sf::VideoMode::getDesktopMode().height / SETTING_ZOOM) - (!height ? this->getTexture()->getSize().y : height)) + y;
	}
	else if(anchor == ANCHOR_BOTTOM_RIGHT)
	{
		new_x += static_cast<float>((sf::VideoMode::getDesktopMode().width / SETTING_ZOOM) - (!width ? this->getTexture()->getSize().x : width)) + x;
		new_y += static_cast<float>((sf::VideoMode::getDesktopMode().height / SETTING_ZOOM) - (!height ? this->getTexture()->getSize().y : height)) + y;
	}
	else if(anchor == ANCHOR_CENTER)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_CENTER_BOTTOM)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>((((sf::VideoMode::getDesktopMode().height / 4) / SETTING_ZOOM) * 3.0f) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_CENTER_LEFT)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 4) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_CENTER_RIGHT)
	{
		new_x += static_cast<float>((((sf::VideoMode::getDesktopMode().width / 4) / SETTING_ZOOM) * 3.0f) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_CENTER_TOP)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 4) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_LEFT)
	{
		new_x += x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_RIGHT)
	{
		new_x += static_cast<float>((sf::VideoMode::getDesktopMode().width / SETTING_ZOOM) - (!width ? this->getTexture()->getSize().x : width)) + x;
		new_y += static_cast<float>(((sf::VideoMode::getDesktopMode().height / 2) / SETTING_ZOOM) - floor((!height ? this->getTexture()->getSize().y : height) / 2)) + y;
	}
	else if(anchor == ANCHOR_TOP)
	{
		new_x += static_cast<float>(((sf::VideoMode::getDesktopMode().width / 2) / SETTING_ZOOM) - floor((!width ? this->getTexture()->getSize().x : width) / 2)) + x;
		new_y += y;
	}
	else if(anchor == ANCHOR_TOP_LEFT)
	{
		new_x += x;
		new_y += y;
	}
	else if(anchor == ANCHOR_TOP_RIGHT)
	{
		new_x += static_cast<float>((sf::VideoMode::getDesktopMode().width / SETTING_ZOOM) - (!width ? this->getTexture()->getSize().x : width)) + x;
		new_y += y;
	}

	this->setPosition(new_x, new_y);
}
