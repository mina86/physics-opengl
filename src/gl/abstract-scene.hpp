/*
 * src/gl/abstract-scene.hpp
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
#ifndef H_ABSTRACT_SCENE_HPP
#define H_ABSTRACT_SCENE_HPP

#include <memory>
#include <iostream>

#include "../lib/lexer.hpp"

namespace mn {

namespace gl {

struct Widget;

struct AbstractScene {
	typedef std::auto_ptr<AbstractScene> ptr;

	virtual ~AbstractScene() { }
	virtual void initializeGL() = 0;
	virtual void drawScene(const Widget &gl) = 0;
	virtual void updateState(unsigned ticks, float dt) = 0;

	virtual void save(std::ostream &out) throw(std::ios_base::failure) = 0;
	static  ptr  load(std::istream &in)
		throw(std::ios_base::failure, lib::Lexer::error);
	static const char extension[];
};

}

}

#endif
