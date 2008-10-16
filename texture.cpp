#include <stdint.h>

#include <fstream>

#include "texture.hpp"


namespace mn {

namespace gl {


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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0,
	             internalFormat, width, height, 0,
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
		const float mul = 1.0 / 255.0 / (width * height);
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
	unsigned long rleEnd;
	uint32_t *rowStart, *rowSize;


	RGBImageReader(const char *filename);
	~RGBImageReader() {
		delete[] rowStart;
		delete[] tmp;
		if (file) fclose(file);
	}

	void getRow(uint8_t *buf, int y, int z);

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
		rleEnd   = 512 + (2 * x * 4);
		fseek(file, 512, SEEK_SET);
		fread(rowStart, 4, x * 2, file);
		if (swapFlag) {
			convertUint(rowStart, x * 2);
		}
	}
}


void RGBImageReader::getRow(uint8_t *buf, int y, int z) {
	uint8_t *iPtr, *oPtr, pixel;
	int count;

	if ((header.type & 0xFF00) != 0x0100) {
		fseek(file, 512 + header.xsize * (y + z * header.ysize), SEEK_SET);
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


void Texture::load(const char *filename) {
	RGBImageReader image(filename);
	if (!image.file) return;

	const unsigned xsize = image.header.xsize;
	const unsigned ysize = image.header.ysize;
	unsigned char *d;
	bool isRGB;

	if (image.header.zsize < 3) {
		isRGB = false;
		data = new unsigned char[xsize * ysize];
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
