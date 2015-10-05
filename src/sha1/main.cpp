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
#include <sha1/version.hpp>
#include <wire/wire.hpp>

int32_t main(int32_t argc, char *argv[])
{
	if(argc == 3)
	{
		auto data = apathy::file(argv[1]).read();

		if(data.size())
		{
			wire::string output_directory(argv[2]);

			auto backslash = output_directory.find_last_of('\\');
			auto slash = output_directory.find_last_of('/');

			if((backslash != wire::string::npos) || (slash != wire::string::npos))
			{
				if((backslash != wire::string::npos) && (slash != wire::string::npos))
					output_directory = output_directory.substr(0, (backslash > slash) ? backslash : slash);

				else if(backslash != wire::string::npos)
					output_directory = output_directory.substr(0, backslash);

				else
					output_directory = output_directory.substr(0, slash);
			}

			apathy::path path(output_directory);

			if(((backslash != wire::string::npos) || (slash != wire::string::npos)) && !path.exists())
				apathy::path::md(path);
			
			auto sha1 = cocoa::SHA1(data);

			if(apathy::file(argv[2]).overwrite(sha1))
				std::cout << "Wrote ";

			else
				std::cout << "Error: Could not write ";

			std::cout << "SHA1 (" << sha1 << ") for '" << argv[1] << "' to file: " << argv[2] << std::endl;
		}
		else
		{
			std::cout << "Error: Could not read from file: " << argv[1] << std::endl;

			return EXIT_FAILURE;
		}
	}
	else
	{
		std::cout << SHA1_NAME << " " << SHA1_VERSION << std::endl;
		std::cout << "Copyright (C) 2013-2015 " << SHA1_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "    " << argv[0] << " <input_file> <output_file>" << std::endl;
	}

	return EXIT_SUCCESS;
}
