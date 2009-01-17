/*
 * src/common/texture.hpp
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
#ifndef H_TEXTURE_HPP
#define H_TEXTURE_HPP

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "color.hpp"

namespace mn {

namespace gl {


struct Texture {
	Texture(GLint theInternalFormat = GL_RGB, GLint theFormat = GL_RGB,
	        GLint theType = GL_UNSIGNED_BYTE)
		: internalFormat(theInternalFormat), format(theFormat), type(theType),
		  width(0), height(0), id(0), data(0) { }

	Texture(unsigned theWidth, unsigned theHeight, unsigned char *theData,
	        GLint theInternalFormat = GL_RGB, GLint theFormat = GL_RGB,
	        GLint theType = GL_UNSIGNED_BYTE)
		: internalFormat(theInternalFormat), format(theFormat), type(theType),
		  width(theWidth), height(theHeight), id(0), data(theData) {
		calculatAverage();
	}

	~Texture() {
		delete[] data;
		if (id) {
			glDeleteTextures(1, &id);
		}
	}

	GLint getInternalFormat() const          { return internalFormat; }
	void setInternalFormat (GLint theFormat) { internalFormat = theFormat; }
	GLint getFormat        () const          { return format; }
	void setFormat         (GLint theFormat) { format = theFormat; }
	GLint getType          () const          { return type; }
	void setType           (GLint theType)   { type = theType; }

	unsigned getWidth () const             { return width; }
	unsigned getHeight() const             { return height; }

	void assign(unsigned theWidth, unsigned theHeight, unsigned char*theData);

	static const char *filename_prefix;
	static const char *filename_suffix;

	void load(const char *filename);
	void free() {
		delete[] data;
		if (id) {
			glDeleteTextures(1, &id);
		}
		data = 0;
		id = 0;
	}

	GLuint get() const {
		if (!id) makeTexture();
		return id;
	}

	GLuint release() {
		GLuint ret = get();
		id = 0;
		return ret;
	}


	const Color &getAverageColor() const { return average; }


	operator bool () const { return data || id; }
	bool operator!() const { return !data && !id; }
	GLuint operator*() const { return get(); }


	static bool useNearest;


private:
	Texture(const Texture &t) { (void)t; }

	void makeTexture() const;
	void calculatAverage();

	GLint internalFormat, format, type;
	unsigned width, height;
	mutable GLuint id;
	mutable unsigned char *data;
	Color average;
};


}

}

#endif
