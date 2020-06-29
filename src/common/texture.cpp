/*
 * src/common/texture.cpp
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
#include "texture.hpp"

#include <stdint.h>

#include <fstream>


namespace mn {

namespace gl {


const char *Texture::filename_prefix = "";
const char *Texture::filename_suffix = ".hq.sgi";
bool Texture::useNearest = false;


void Texture::assign(unsigned theWidth, unsigned theHeight,
                     unsigned char *theData) {
	delete[] data;
	width = theWidth;
	height = theHeight;
	data = theData;
	calculatAverage();
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
	if (!data) return;
	if (!id) {
		glGenTextures(1, &id);
	}
	glBindTexture(GL_TEXTURE_2D, id);
	if (useNearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	gluBuild2DMipmaps(GL_TEXTURE_2D, internalFormat, width, height,
	                  format, type, data);

	delete[] data;
	data = 0;
}


void Texture::calculatAverage() {
	if (!data || !width || !height) {
		fprintf(stderr, "QWEQWE!\n");
		return;
	}

	if (format == GL_LUMINANCE) {
		float avg = 0;
		for (unsigned char *it=data, *end=it+width*height; it!=end; ++it) {
			avg += *it;
		}
		average.r = average.g = average.b = avg / 255 / (width * height);
	} else {
		float r = 0, g = 0, b = 0;
		for (unsigned char *it=data, *end=it+width*height*3; it!=end; ) {
			r += *it++;
			g += *it++;
			b += *it++;
		}
		const float mul = 1.0f / 255.0f / (width * height);
		average.r = r * mul;
		average.g = g * mul;
		average.b = b * mul;
	}
}



namespace {


struct RGBImageReader {
	struct {
		uint16_t imagic;
		uint16_t type;
		uint16_t dim;
		uint16_t xsize, ysize, zsize;
		uint32_t min, max;
		uint32_t wasteBytes;
		char name[80];
		uint32_t colorMap;
	} header;
	FILE *file;
	uint8_t *tmp;
	uint32_t *rowStart, *rowSize;


	RGBImageReader(const char *filename);
	~RGBImageReader() {
		delete[] rowStart;
		delete[] tmp;
		if (file) fclose(file);
	}

	void getRow(uint8_t *buf, unsigned y, unsigned z);

	static void convertShort(uint16_t *array, unsigned length) {
		unsigned short b1, b2;
		uint8_t *ptr;

		ptr = (uint8_t *)array;
		while (length--) {
			b1 = *ptr++;
			b2 = *ptr++;
			*array++ = (b1 << 8) | (b2);
		}
	}

	static void convertUint(uint32_t *array, unsigned length) {
		unsigned int b1, b2, b3, b4;
		uint8_t *ptr;

		ptr = (uint8_t *)array;
		while (length--) {
			b1 = *ptr++;
			b2 = *ptr++;
			b3 = *ptr++;
			b4 = *ptr++;
			*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
		}
	}
};


RGBImageReader::RGBImageReader(const char *filename)
	: tmp(0), rowStart(0), rowSize(0) {

	union { int testWord; char testByte[4]; } endianTest;
	endianTest.testWord = 1;
	const bool swapFlag = endianTest.testByte[0] == 1;

	if (!(file = fopen(filename, "rb"))) {
		perror(filename);
		return;
	}
	fread(&header, 1, 12, file);

	if (swapFlag) {
		convertShort(&header.imagic, 6);
	}

	if ((header.type & 0xFF00) == 0x0100) {
		unsigned x = header.ysize * header.zsize;
		tmp      = new uint8_t[header.xsize * 256];
		rowStart = new uint32_t[x * 2];
		rowSize  = rowStart + x;
		fseek(file, 512, SEEK_SET);
		fread(rowStart, 4, x * 2, file);
		if (swapFlag) {
			convertUint(rowStart, x * 2);
		}
	}
}


void RGBImageReader::getRow(uint8_t *buf, unsigned y, unsigned z) {
	uint8_t *iPtr, *oPtr, pixel;
	int count;

	if ((header.type & 0xFF00) != 0x0100) {
		fseek(file, 512u + header.xsize * (y + z * header.ysize),
		      SEEK_SET);
		fread(buf, 1, header.xsize, file);
		return;
	}

	fseek(file, rowStart[y + z * header.ysize], SEEK_SET);
	fread(tmp, 1, rowSize[y + z * header.ysize], file);

	iPtr = tmp;
	oPtr = buf;
	for (;;) {
		pixel = *iPtr++;
		count = (int)(pixel & 0x7F);
		if (!count) return;
		if (pixel & 0x80) {
			while (count--) *oPtr++ = *iPtr++;
		} else {
			pixel = *iPtr++;
			while (count--) *oPtr++ = pixel;
		}
	}
}

}


void Texture::load(const char *filename_) {
	char filename[1024];
	snprintf(filename, sizeof filename, "%s%s%s",
	         filename_prefix, filename_, filename_suffix);

	RGBImageReader image(filename);
	if (!image.file) return;

	const unsigned xsize = image.header.xsize;
	const unsigned ysize = image.header.ysize;
	unsigned char *d;
	bool isRGB;

	if (image.header.zsize < 3) {
		isRGB = false;
		d = new unsigned char[xsize * ysize];
		unsigned char *optr = d;
		for (unsigned y = ysize; y--; ) {
			image.getRow(optr, y, 0);
			optr += xsize;
		}
	} else {
		isRGB = true;
		uint8_t *const rbuf = new uint8_t[xsize * 3];
		uint8_t *const gbuf = rbuf + xsize;
		uint8_t *const bbuf = gbuf + xsize;
		d = new unsigned char[xsize * ysize * 3];
		unsigned char *optr = d;

		for (unsigned y = ysize; y--; ) {
			image.getRow(rbuf, y, 0);
			image.getRow(gbuf, y, 1);
			image.getRow(bbuf, y, 2);

			uint8_t *r = rbuf, *g = gbuf, *b = bbuf;
			for (unsigned n = xsize; n--; ) {
				*optr++ = *r++;
				*optr++ = *g++;
				*optr++ = *b++;
			}
		}
		delete[] rbuf;
	}

	internalFormat = format = isRGB ? GL_RGB : GL_LUMINANCE;
	type = GL_UNSIGNED_BYTE;

	assign(xsize, ysize, d);
}


}

}
