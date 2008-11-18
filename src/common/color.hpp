#ifndef H_COLOR_HPP
#define H_COLOR_HPP

namespace mn {

namespace gl {


struct Color {
	float r, g, b;
};


inline Color color() {
	Color c;
	c.r = c.g = c.b = 1;
	return c;
}

inline Color color(float r, float g, float b) {
	Color c;
	c.r = r;
	c.g = g;
	c.b = b;
	return c;
}

inline Color color(unsigned long color) {
	Color c;
	c.r = ((color >> 16) & 255) / 255.0f;
	c.g = ((color >>  8) & 255) / 255.0f;
	c.b = ( color        & 255) / 255.0f;
	return c;
}


}

}

#endif
