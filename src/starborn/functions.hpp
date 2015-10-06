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

namespace ss
{
	bool file_exists(wire::string filename);
	bool update_file(wire::string source_filename, wire::string sha1_url, wire::string destination_url, bool delete_old_file = true, bool unpack = true, std::function<void (wire::string &filename)> callback = [](wire::string &filename){});
	bool update_files(std::vector<wire::string> &files, std::vector<wire::string> &critical_files, wire::string github_url, std::function<void (uint32_t file, wire::string &filename)> callback = [](uint32_t file, wire::string &filename){});

	bundle::string unpack_asset(bundle::file &asset);
	bundle::string unpack_asset(wire::string data, wire::string &asset_filename);

	void handle_updated(std::vector<wire::string> &old_critical_files);
	void log(bool open, bool feed, bool close, const std::string &line);
	void log_cpu_usage();
	void stack_trace();

	wire::string get_timestamped_filename(wire::string directory, wire::string filename_prefix, wire::string extension);
}
