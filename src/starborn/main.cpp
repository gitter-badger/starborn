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
	apathy::ostream::attach(std::cout, &ss::Starborn::log);

	ss::Starborn().run();

	std::cout << std::endl;
	std::cout << "CPU usage for this session:" << std::endl;
	std::cout << std::endl;

	profit::report(std::cout);
	apathy::ostream::detach(std::cout);

	return EXIT_SUCCESS;
}
