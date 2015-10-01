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

int32_t _stdcall WinMain()
{ $
	ss::vectors::Strings critical_files =
	{
		"starborn.pdb", "starborn.exe"
	};

	ss::vectors::Strings files =
	{
		"assets.zip", "base91.exe", "bundler.exe", "uuid.exe"
	};

	ss::utilities::handle_updated(critical_files);
	apathy::ostream::attach(std::cout, &ss::utilities::log);

	std::cout << STARBORN_NAME << " " << STARBORN_VERSION << std::endl;
	std::cout << "Copyright (C) 2013-2015 " << STARBORN_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
	std::cout << std::endl;
	std::cout << "[" << GIT_BRANCH << "] " << __DATE__ << " " << __TIME__ << std::endl;
	std::cout << std::endl;

	if(!ss::utilities::update_files(files, critical_files, "https://github.com/snailsoft/starborn/blob/" GIT_BRANCH "/patch/"))
		ss::Starborn().run();

	ss::utilities::log_cpu_usage();
	apathy::ostream::detach(std::cout);

	return EXIT_SUCCESS;
}
