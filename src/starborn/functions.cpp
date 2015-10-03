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
#include <windows.h>

std::ofstream logger;
wire::string logger_cache;

bool ss::update_file(wire::string source_filename, wire::string sha1_url, wire::string destination_url, bool delete_old_file, std::function<void (wire::string &filename)> callback)
{ $
	apathy::file file(source_filename);

	if(!file.exists() || (SHA1::from_file(source_filename) != flow::download(sha1_url).data))
	{ $
		if(file.exists())
			file.patch(base91::decode(flow::download(destination_url).data), delete_old_file);

		else
			file.overwrite(base91::decode(flow::download(destination_url).data));

		callback(source_filename);
		
		return true;
	}

	return false;
}

bool ss::update_files(std::vector<wire::string> &files, std::vector<wire::string> &critical_files, wire::string github_url, std::function<void (uint32_t file, wire::string &filename)> callback)
{ $
	auto revision = wire::string(flow::download(github_url + "revision.txt").data).as<int32_t>();
	uint32_t files_updated = 0;

	for(auto &&file : files)
	{ $
		if(!apathy::file(file).exists() || (GIT_REVISION_NUMBER < revision))
			update_file(file, github_url + file + ".sha1", github_url + file + ".b91");

		callback(++files_updated, file);
	}

	auto restart_required = false;

	for(auto &&critical_file : critical_files)
	{ $
		if(!apathy::file(critical_file).exists() || (GIT_REVISION_NUMBER < revision))
		{ $
			if(update_file(critical_file, github_url + critical_file + ".sha1", github_url + critical_file + ".b91", false))
				restart_required = true;

			callback(++files_updated, critical_file);
		}
	}

	if(restart_required)
	{ $
		PROCESS_INFORMATION process_info;
		STARTUPINFO startup_info;

		memset(&process_info, 0, sizeof(process_info));
		memset(&startup_info, 0, sizeof(startup_info));

		startup_info.cb = sizeof(startup_info);

		char filename[1024];
		memset(filename, 0, sizeof(filename));

		GetModuleFileName(GetModuleHandle(nullptr), filename, sizeof(filename));
		CreateProcess(nullptr, filename, nullptr, nullptr, false, 0, nullptr, nullptr, &startup_info, &process_info);

		return true;
	}

	return false;
}

bundle::string ss::unpack_asset(bundle::file &asset)
{ $
	auto data = asset["data"];

	if(bundle::is_packed(data))
		bundle::unpack(data, asset["data"]);

	return data;
}

void ss::handle_updated(std::vector<wire::string> &old_critical_files)
{ $
	auto updated = false;

	for(auto &&old_critical_file : old_critical_files)
	{ $
		apathy::file old_critical_file(old_critical_file + ".$old");

		if(old_critical_file.exists())
		{ $
			while(old_critical_file.exists())
				old_critical_file.remove();

			updated = true;
		}
	}

	if(updated)
		bubble::notify("Updated to version " STARBORN_VERSION, STARBORN_NAME);
}

void ss::log(bool open, bool feed, bool close, const std::string &line)
{ $
	if(open || close)
	{ $
		if(open)
			logger.open(get_timestamped_filename("logs", "starborn", ".txt").c_str(), std::ios::app | std::ios::binary | std::ios::out);
		
		logger << line << std::endl;

		if(close)
			logger.close();
	}
	else if(feed)
	{ $
		logger << sand::format(sand::now(), "[mm/dd/yyyy HH:MM:SS] ") << logger_cache << "\r\n";
		logger.flush();
		logger_cache.clear();
	}
	else
		logger_cache += line;
}

void ss::log_cpu_usage()
{ $
	std::cout << std::endl;
	std::cout << "CPU usage for this session:" << std::endl;
	std::cout << std::endl;

	profit::report(std::cout);
}

void ss::stack_trace()
{ $
	metrics::table_ascii table;
	table.add_column_right("#").add_column_left("function").with_horizontal_padding(1);

	auto call_stack = heal::stacktrace("\2", 3);

	for(auto i = 0; i < call_stack.size(); ++i)
		table << i << call_stack[i];

	table.print(std::cout, false);
}

wire::string ss::get_timestamped_filename(wire::string directory, wire::string filename_prefix, wire::string extension)
{ $
	apathy::path path(directory);

	if(!path.exists())
		apathy::path::md(path);

	return directory + "/" + filename_prefix + "-" + sand::format(sand::now(), "mm-dd-yy-HH-MM-SS") + extension;
}
