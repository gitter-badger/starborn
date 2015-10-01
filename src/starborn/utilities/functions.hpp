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
	namespace vectors
	{
		typedef std::vector<wire::string> Strings;
	};

	namespace utilities
	{
		bool update_file(wire::string source_filename, wire::string sha1_url, wire::string destination_url, bool delete_old_file = true);
		bool update_files(vectors::Strings &files, vectors::Strings &critical_files, wire::string github_url);

		bundle::string unpack_asset(bundle::file &asset);

		void handle_updated(vectors::Strings &old_critical_files);
		void log(bool open, bool feed, bool close, const std::string &line);
		void log_cpu_usage();
		void stack_trace();

		wire::string get_filename(wire::string directory, wire::string filename_prefix, wire::string extension);
	}
}
