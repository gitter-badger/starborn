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

rapidjson::Document &ss::Json::get_document()
{ $
	return this->document;
}

ss::Json::Json(bundle::string &json_data, bool file)
{ $
	this->parse(json_data, file);
}

void ss::Json::parse(bundle::string &json_data, bool file)
{ $
	if(file)
	{ $
		auto *file_handle = fopen(json_data.c_str(), "r");
		char buffer[256];

		rapidjson::FileReadStream stream(file_handle, buffer, sizeof(buffer));
		this->document.ParseStream(stream);

		if(file_handle)
			fclose(file_handle);
	}
	else
		this->document.Parse(json_data.c_str());
}
