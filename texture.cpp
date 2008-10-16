#include <stdio.h>

#include "texture.hpp"


namespace mn {

namespace gl {


void Texture::assign(unsigned theWidth, unsigned theHeight,
                     unsigned char *theData) {
	delete[] data;
	width = theWidth;
	height = theHeight;
	data = theData;
	if (id) {
		if (data) {
			makeTexture();
		} else {
			glDeleteTextures(1, &id);
			id = 0;
		}
	}
}


void Texture::makeTexture() const {
	fprintf(stderr, "asd\n");
	if (!data) return;
	if (!id) {
		glGenTextures(1, &id);
	}

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0,
	             internalFormat, width, height, 0,
	             format, type, data);

	delete[] data;
	data = 0;
}


}

}
