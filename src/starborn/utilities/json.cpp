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

rapidjson::Document &ss::utilities::Json::get_document()
{
	return this->document;
}

ss::utilities::Json::Json(std::string filename)
{
	this->parse(filename);
}

void ss::utilities::Json::parse(std::string filename)
{
	auto *file = fopen(filename.c_str(), "r");
	char buffer[256];

	rapidjson::FileReadStream stream(file, buffer, sizeof(buffer));
	this->document.ParseStream(stream);

	if(file)
		fclose(file);
}
