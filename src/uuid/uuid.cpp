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

#include <git.hpp>
#include <iostream>
#include <sole/sole.hpp>
#include <uuid/version.hpp>

int32_t main(int32_t argc, char *argv[])
{
    if(argc == 1)
        std::cout << sole::uuid4() << std::endl;

    else
    {
        std::cout << UUID_NAME << " " << UUID_VERSION << std::endl;
        std::cout << "Copyright (C) 2013-2015 " << UUID_AUTHOR << " <https://github.com/snailsoft/starborn/>" << std::endl;
        std::cout << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    " << argv[0] << std::endl;
    }

    return EXIT_SUCCESS;
}
