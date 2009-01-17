/*
 * src/common/quadric.cpp
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
#include "quadric.hpp"

namespace mn {

namespace gl {


Quadric *Quadric::singleton = 0;


void Quadric::setNextDrawStyle() {
	GLenum ds;
	switch (drawStyle) {
	case GLU_FILL: ds = GLU_LINE; break;
	case GLU_LINE: ds = GLU_POINT; break;
	default: ds = GLU_FILL; break;
	}
	setDrawStyle(ds);
}


}

}
