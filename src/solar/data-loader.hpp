/*
 * src/solar/data-loader.hpp
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
#ifndef H_DATA_LOADER_HPP
#define H_DATA_LOADER_HPP

#include <fstream>
#include <stdexcept>
#include <vector>

namespace mn {

namespace solar {

struct Sphere;


/**
 * Loads objects speciication from a given file.
 *
 * The file specifies a hierarchical tree of spheres with their
 * properties.  It has a fairly simple syntax and it's grammar is:
 *
 * <pre>
 * input     : [ factors ] object
 *
 * factors   : "factors" distance-factor size-factor year-factor day-factor
 *
 * object    : name properties appearance "{" { object } "}"
 * properties: distance size year day
 * appearance: "(" red green blue ")" [ "texture" texture-file ] [ "light" ]
 * </pre>
 *
 * Factors specify value that given properties will be multiplicated
 * by after being read.
 *
 * Objects are arranged in a tree structure and therefore e will use
 * a "standard" terminology, ie. parent-children.  The root sphere is
 * located in the centre of the rendered animation and it does not
 * move (excluding the "day" movement).
 *
 * All other spheres move around their parents.  <i>distance</i>
 * specify how far away the object is from its parent, <i>size</i> its
 * radius when rendered, <i>year</i> how long does it take it to move
 * around its parent and <i>day</i> how long does it take to move
 * around its own axis (the last attribute is ignored if no texture is
 * specified or texture rendering is disabled).  <i>red</i>,
 * <i>green</i> and <i>blue</i> components it's surface colour.  If
 * texture is specified loader tries to read given texture which will
 * be used when rendering body.  Note also, that when texture is given
 * object's color is calculated automatically from the texture (an
 * average color of the image is taken).  The light attribute, when
 * given, says that the object is a light source.
 *
 * \param filename file name of the file with configuration.
 */
Sphere *loadData(const std::string &filename);

}

}

#endif
