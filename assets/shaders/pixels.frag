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

uniform float time;
uniform vec2 resolution;

float random(vec2 value) 
{
	return fract(sin(dot(value, vec2(3.9898, 78.233))) * (10000.0 + (time * 0.05)));
}

void main() 
{
	float rand = random(floor(((gl_FragCoord * 0.0139) + vec2(time * 1.5, 0.0)) + 2.0));	
	float value = (rand < 0.05) ? rand : random(floor(((gl_FragCoord * 0.042) + vec2(time, 0.0)) + 4.0));
	
	gl_FragColor = vec4(((value < 0.05) ? vec3(0.0, 1.0, 0.5) : mix(vec3(0.0), vec3(0.0, 1.0, 1.0), pow(value, 8.0))) * (1.6 - length(((gl_FragCoord / resolution) * 2.0) - 1.0)), (time < 3.0) ? min(1.0, time) : max(0.0, 1.0 - (time - 3.0)));
}
