/*
 * src/gl/texture.hpp
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
#ifndef H_TEXTURE_HPP
#define H_TEXTURE_HPP

#include <string>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

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
		free();
	}

	GLint getInternalFormat() const          { return internalFormat; }
	void setInternalFormat (GLint theFormat) { internalFormat = theFormat; }
	GLint getFormat        () const          { return format; }
	void setFormat         (GLint theFormat) { format = theFormat; }
	GLint getType          () const          { return type; }
	void setType           (GLint theType)   { type = theType; }

	unsigned getWidth () const             { return width; }
	unsigned getHeight() const             { return height; }

	void assign(unsigned theWidth, unsigned theHeight, unsigned char *theData);

	static const char *filename_prefix;
	static const char *filename_suffix;

	void load(const std::string &filename) throw();
	void free();

	GLuint get() const {
		if (!id) makeTexture();
		return id;
	}

	GLuint release() {
		GLuint ret = get();
		id = 0;
		return ret;
	}

	const float *getAverageColor() const { return average; }

	operator bool () const { return data || id; }
	bool operator!() const { return !data && !id; }
	GLuint operator*() const { return get(); }

	static bool useNearest;

	void makeStars();
	static unsigned starsQuality;

private:
	Texture(const Texture &t) { (void)t; }

	void makeTexture() const;
	void calculatAverage();

	GLint internalFormat, format, type;
	unsigned width, height;
	mutable GLuint id;
	mutable unsigned char *data;
	float average[3];
};

}

#endif
