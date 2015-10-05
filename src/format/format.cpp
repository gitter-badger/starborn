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

#include <apathy/apathy.hpp>
#include <cocoa/cocoa.hpp>
#include <git.hpp>
#include <format/version.hpp>
#include <regex>
#include <wire/wire.hpp>

int32_t main(int32_t argc, char *argv[])
{
	if(argc > 2)
	{		
		std::vector<std::string> masks;

		for(auto i = 2; i < argc; ++i)
			masks.push_back(argv[i]);

		apathy::folder directory;
		directory.include(argv[1], masks);

		for(auto &&file : directory)
		{
			wire::string data = file.read();

			if(data.size())
			{
				auto original_sha1 = cocoa::SHA1(data);
				data = std::regex_replace(data.replace("{ $", "{").replace("{", "{ $"), std::regex("=(\\s*)\\{ \\$"), "=$1{");
				
				if(original_sha1 != cocoa::SHA1(data))
				{
					if(file.overwrite(data))
						std::cout << "Formatted file: " << file.name() << std::endl;

					else
						std::cout << "Warning: Could not write to file: " << file.name() << std::endl;
				}
			}
			else
				std::cout << "Warning: Could not read from file: " << file.name() << std::endl;
		}
	}
	else
	{
		std::cout << FORMAT_NAME << " " << FORMAT_VERSION << std::endl;
		std::cout << "Copyright (C) 2013-2015 " << FORMAT_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "    " << argv[0] << " <directory> <*.cpp> [*.cc] [...]" << std::endl;
	}

	return EXIT_SUCCESS;
}
