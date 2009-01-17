/*
 * src/common/color.hpp
 * Copyright 2009 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef H_COLOR_HPP
#define H_COLOR_HPP

namespace mn {

namespace gl {


struct Color {
	float r, g, b;
};


inline Color color() {
	Color c;
	c.r = c.g = c.b = 1;
	return c;
}

inline Color color(float r, float g, float b) {
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

inline Color color(unsigned long color) {
	Color c;
	c.r = ((color >> 16) & 255) / 255.0f;
	c.g = ((color >>  8) & 255) / 255.0f;
	c.b = ( color        & 255) / 255.0f;
	return c;
}


}

}

#endif
