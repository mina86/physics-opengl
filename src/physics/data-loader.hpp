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
