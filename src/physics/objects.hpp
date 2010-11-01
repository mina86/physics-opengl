/*
 * src/physics/objects.hpp
 * Copyright 2010 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
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
#ifndef H_OBJECTS_HPP
#define H_OBJECTS_HPP

#include "../common/abstract-objects.hpp"

namespace mn {

namespace physics {

struct Object;

struct Objects : public gl::AbstractObjects {
	Objects(Object *theObjects) : objects(theObjects) { }
	~Objects();

	void initializeGL();
	void drawScene(const gl::Widget &gl);
	void updateState(unsigned ticks, float dt);

private:
	Objects(const Objects &);
	void operator=(const Object &);

	void updateStateOnce(float dt);
	void updatePointAll();

	Object *objects;
};

}

}

#endif
