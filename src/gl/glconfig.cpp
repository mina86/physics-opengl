/*
 * src/gl/glconfig.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#include "glconfig.hpp"

#include "../lib/mconst.h"

namespace gl {

namespace cfg {

const char *const drawStyles[] = {
	"Fill", "Lines", "Dots", NULL
};

Data::Data()
	: mouseMovementFactor("Movement sensitivity", 0.001, 1.0, 0.01),
	  mouseRotationFactor("Rotation sensitivity", 1.0, 100.0, 10.0),
	  runFactor("'Run' (when Ctrl held down) multiplier", 1.0, 100, 10.0),
	  creepFactor("'Creep' (when Shift held down) multiplier", 0.01, 1, 0.1),
	  cutOffDistance2("Square of distance when to start degrading quality",
					  100, (MAX_DISTANCE * 2.0) * (MAX_DISTANCE * 2.0),
					  2500.0),
	  showText("Show names above objects", true),
	  showTextures("Display textures", true),
	  lowQuality("Use low quality rendering", false),
	  showStars("Show stars (disable if rendering too slow)", true),
	  drawStyle("Object draw style", drawStyles) {
	connect(&drawStyle, SIGNAL(changed(long)),
	        this, SLOT(drawStyleChanged(long)));
	init();
}

ui::cfg::Data::iterator Data::items() const {
	static const unsigned array[] = {
		CFG_DATA_OFFSET(cutOffDistance2),
		CFG_DATA_OFFSET(showText),
		CFG_DATA_OFFSET(showTextures),
		CFG_DATA_OFFSET(lowQuality),
		CFG_DATA_OFFSET(showStars),
		CFG_DATA_OFFSET(drawStyle),
		~0u,
	};
	return iterator(array);
}

void Data::drawStyleChanged(ui::cfg::List::value_type v) {
	GLenum ds;
	switch (v) {
	case  0: ds = GLU_FILL; break;
	case  1: ds = GLU_LINE; break;
	default: ds = GLU_POINT; break;
	}
	quad.setDrawStyle(ds);
}

}

}
