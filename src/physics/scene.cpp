/*
 * src/physics/scene.cpp
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
#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif

#include "object.hpp"
#include "scene.hpp"

namespace mn {

namespace physics {

Scene::~Scene() {
	Object *o = objects;
	do {
		Object *n = o->getNext();
		delete o;
		o = n;
	} while (o != objects);
}

void Scene::initializeGL() {
}

void Scene::drawScene(const gl::Widget &gl) {
	Object *o = objects;
	do {
		o->draw(gl);
		o = o->getNext();
	} while (o != objects);
}

void Scene::updateState(unsigned ticks, float dt) {
	for (; ticks; --ticks) {
		updateStateOnce(dt);
		updatePointAll();
	}
}

void Scene::updateStateOnce(float dt) {
	Object *o = objects;
	do {
		o->tick(dt);
		o = o->getNext();
	} while (o != objects);
}

void Scene::updatePointAll() {
	Object *o = objects;
	do {
		o->updatePoint();
		o = o->getNext();
	} while (o != objects);
}

}

}
