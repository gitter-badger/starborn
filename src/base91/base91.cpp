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
#include <base91/base91.hpp>
#include <base91/version.hpp>
#include <git.hpp>
#include <wire/wire.hpp>

int32_t main(int32_t argc, char *argv[])
{
	if((argc == 4) && ((wire::string(argv[1]) == "-d") || (wire::string(argv[1]) == "-e")))
	{
		auto data = apathy::file(argv[2]).read();

		if(data.size())
		{
			auto base91 = (wire::string(argv[1]) == "-d") ? base64::decode(data) : base64::encode(data);

			if(apathy::file(argv[3]).overwrite(base91))
				std::cout << ((wire::string(argv[1]) == "-d") ? "Decoded " : "Encoded ");

			else
				std::cout << "Error: Could not write ";

			std::cout << std::setprecision(2) << std::fixed << ((base91.size() / 1024.0f) / 1024.0f) << " mb for '" << argv[2] << "' to file: " << argv[3] << std::endl;
		}
		else
		{
			std::cout << "Error: Could not read from file: " << argv[2] << std::endl;

			return EXIT_FAILURE;
		}
	}
	else
	{
		std::cout << BASE91_NAME << " " << BASE91_VERSION << std::endl;
		std::cout << "Copyright (C) 2013-2015 " << BASE91_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
		std::cout << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << "    " << argv[0] << " [options] <input_file> <output_file>" << std::endl;
		std::cout << "Options:" << std::endl;
		std::cout << "    -d        decode file" << std::endl;
		std::cout << "    -e        encode file" << std::endl;
	}

	return EXIT_SUCCESS;
}
