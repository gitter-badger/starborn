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

uniform float pi;
uniform float time;

uniform vec2 resolution;

void main()
{
	vec2 position = (gl_FragCoord - (resolution / 2.0)) / resolution.y;
	vec2 position2 = (gl_FragCoord - (0.7 *resolution)) / (resolution.y / 32.0);

	position2.x = (gl_FragCoord.x - (0.47 * resolution.y)) / (resolution.y / 32.0);

	position /= cos(1.5 * length(position));
	position2 /= cos(1.5 * length(position2));

	gl_FragColor = vec4(sqrt(pow(((((1.0 / ((64.0 * mod(-atan(position.y, position.x) + ((time / 0.5) * (length(position) * length(position))), pi * 6.0)) + 0.2)) * (1.0 - step(0.42, length(position)))) + ((1.0 / ((0.125 * mod(-atan(position2.y, position2.x) + ((time / 0.025) * (length(position2) * length(position2))), pi * 0.6)) + 0.2)) * (1.0 - step(0.42, length(position2))))) + (0.1 / distance(length(position2), 0.42))) + (1.0 / ((75.0 * distance(length(position), 0.42)) + 0.2)), 1.5)) * (vec3(0.0, 1.0, 1.0) * 0.3), (((time - 8.0) < 3.0) ? min(1.0, (time - 8.0)) : 1.0) * 1.0);
}
