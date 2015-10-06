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

uniform float fade_time;
uniform float time;

uniform vec2 resolution;

float random(vec2 value)
{
	return fract(sin(dot(value, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	float rand = 0.0;
	float star = 0.0;

	if(random(gl_FragCoord / resolution) > 0.995)
	{
		rand = random(gl_FragCoord);
		star = rand * (0.225 * sin((time * (rand * 5.0)) + (720.0 * rand)) + 0.5);
	}

	gl_FragColor = vec4(vec3((rand < 0.8) ? vec3(0.0, 1.0, 1.0) : ((rand < 0.85) ? vec3(1.0, 0.0, 0.5) : vec3(0.0, 1.0, 0.5))) * (1.6 - length(((gl_FragCoord / resolution) * 2.0) - 1.0)), ((fade_time > 0.0) ? max(0.0, 1.0 - (time - fade_time)) : ((time < 4.0) ? min(1.0, time) : 1.0)) * star);
}
