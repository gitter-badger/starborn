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

void main()
{
	vec2 position = (gl_FragCoord - (resolution / 2.0)) / resolution.x;

	float angle = fract(atan(position.y, position.x) / acos(-1.0));      
	float color = 0.0;
	
	for(float i = 0.0; i < 5.0; ++i)
	{
		float angle2 = floor(angle * 360.0) + 1.0;
		float radian_distance = sqrt(fract(angle2 * fract(angle2 * 0.82657) * 13.724) + i);            
		float distance = (radian_distance / length(position)) * 0.1;
		
		color += (((max(0.0, 0.5 - (((abs(fract(((time + (fract(angle2 * fract(angle2 * 0.7235) * 45.1) * 100.0)) * 0.2) + distance) - 0.5)) * 25.0) / distance)) * (0.5 - abs(fract(angle * 360.0) - 0.5))) * 5.0) / distance) / radian_distance;
	}

	gl_FragColor = vec4((vec3(color) * vec3(0.0, 1.0, 1.0)) * (1.6 - length(((gl_FragCoord / resolution) * 2.0) - 1.0)), (time < 4.0) ? min(1.0, time) : max(0.0, 1.0 - (time - 4.0))) * 0.3;
}
    