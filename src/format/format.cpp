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
        auto inject_profiler_code = wire::string(argv[1]) == "-p";

        std::vector<std::string> masks;

        for(auto i = (inject_profiler_code ? 3 : 2); i < argc; ++i)
            masks.push_back(argv[i]);

        apathy::folder directory;
        directory.include(argv[inject_profiler_code ? 2 : 1], masks);

        for(auto &&file : directory)
        {
            wire::string data = file.read();

            if(data.size())
            {
                auto original_sha1 = cocoa::SHA1(data);

                if(inject_profiler_code)
                    data = std::regex_replace(data.replace("{ $", "{").replace("{", "{ $"), std::regex("=(\\s*)\\{ \\$"), "=$1{");

                data = std::regex_replace(data.replace("\t", "    "), std::regex("(.?)([ \t]+)(\r?\n)+"), "$1$3");

                auto columns = 0;
                auto length = 0;
                auto line = 0;
                auto start = false;

                for(auto i = 0; i < data.size(); ++i)
                {
                    if((data[i] == '\n') || (i == (data.size() - 1)))
                    {
                        line++;

                        if(columns > 80)
                        {
                            std::cout << "Warning: Line is longer than 80 characters in file: " << file.name() << ":" << line << std::endl;
                            std::cout << "    => " << data.substr(i - length, length) << std::endl;
                        }

                        columns = 0;
                        length = 0;
                        start = false;
                    }
                    else
                    {
                        columns += (data[i] == '\t') ? 8 : 1;

                        if((start && ((data[i] == ' ') || (data[i] == '\t'))) || ((data[i] != ' ') && (data[i] != '\t')))
                        {
                            length++;

                            if(!start && ((data[i] != ' ') && (data[i] != '\t')))
                                start = true;
                        }
                    }
                }

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
        std::cout << "    " << argv[0] << " [options] <directory> <*.cpp> [*.cc] [...]" << std::endl;
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "    -p        inject cpu profiling code" << std::endl;
    }

    return EXIT_SUCCESS;
}
