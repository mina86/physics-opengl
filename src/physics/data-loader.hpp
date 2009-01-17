/*
 * src/physics/data-loader.hpp
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

namespace physics {

struct Object;

/**
 * Loads objects speciication from a given file.
 *
 * The file specifies a set of objects with their properties.  It has
 * a fairly simple syntax and it's grammar is:
 *
 * <pre>
 * input    : { factors | object }
 *
 * factors  : "*" { factor }
 * factor   : "@"    NUMBER             // position factor
 *          | "vel"  NUMBER             // velocity factor
 *          | "mass" NUMBER             // mass     factor
 *          | "size" NUMBER             // size     factor
 *
 * object   : STRING { attribute }
 * attribute: "@" NUMBER NUMBER NUMBER  // position
 *          | "x" NUMBER                // position (x)
 *          | "y" NUMBER                // position (y)
 *          | "z" NUMBER                // position (z)
 *          | "vel" NUMBER NUMBER NUMBER [ "auto" STRING ]  // velocity
 *          | "frozen"
 *          | "mass" NUMBER
 *          | "size" NUMBER
 *          | "color" NUMBER NUMBER NUMBER
 *          | "texture" STRING
 *          | "light"
 * </pre>
 *
 * Factors specify value that given properties will be multiplicated
 * by after being read.  This allow (for example) easy unit
 * conversion.
 *
 * Object is defined by specifying it's name followed by its
 * properties.
 *
 * When specifying position either all three values (x, y and z) may
 * be given at once or each of them individually -- if a given value
 * is not given it is assumed to be zero.  Position should be given in
 * meters.
 *
 * Velocity is specified by three values (in meters per second) but it
 * may be followed by a keyword <tt>auto</tt> followed by an object
 * name (which must be defined earlier) -- if that is the case the
 * value of the velocity is calculated from the equation \f$ V^2 = a/r
 * = Gm'/r \f$.  As an alternative one can use a <tt>frozen</tt>
 * keyword in which case object will not move at all.
 *
 * Mass specify object's mass in kilo grams.
 *
 * Other attributes affect only the way objects are displayed.  Size
 * specify it's radius (in meters) and color it's surface color.  If
 * texture is specified loader tries to read given texture which will
 * be used when rendering body.  Note also, that when texture is given
 * object's color is calculated automatically from the texture (an
 * average color of the image is taken).  The light attribute, when
 * given, says that the object is a light source.
 *
 * \param filename file name of the file with configuration.
 */
Object *loadData(const char *filename);

}

}

#endif
