/*
 * Copyright (C) 2015 Snailsoft <https://github.com/snailsoft/starborn/>
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

#include <SFML/Graphics.hpp>

#include <Thor/Animations.hpp>
#include <Thor/Input.hpp>
#include <Thor/Resources.hpp>

#include <starborn/constants/actions.hpp>
#include <starborn/constants/assets.hpp>
#include <starborn/constants/drawable_types.hpp>
#include <starborn/constants/settings.hpp>
#include <starborn/constants/states.hpp>

#include <starborn/entities/animated_sprite.hpp>
#include <starborn/entities/state.hpp>

namespace ss
{
	class Starborn
	{
		private:
			entities::State state;

			sf::RenderWindow window;
			sf::View view;

			thor::ActionMap<std::string> actions;
			thor::ActionMap<std::string>::CallbackSystem callbacks;

			thor::ResourceHolder<sf::Texture, std::string> assets;

			void on_exit();

		public:
			Starborn();

			void run();
	};
}
