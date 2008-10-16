#ifndef H_TEXTURE_HPP
#define H_TEXTURE_HPP

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif


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
		  width(theWidth), height(theHeight), id(0), data(theData) { }

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


	operator bool () const { return data || id; }
	bool operator!() const { return !data && !id; }
	GLuint operator*() const { return get(); }


private:
	Texture(const Texture &t) { (void)t; }

	void makeTexture() const;

	GLint internalFormat, format, type;
	unsigned width, height;
	mutable GLuint id;
	mutable unsigned char *data;
};


}

}

#endif
