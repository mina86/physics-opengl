#ifndef H_GLPANE_HPP
#define H_GLPANE_HPP

#include <memory>

#include <QWidget>

#include "config.hpp"
#include "abstract-objects.hpp"

struct QSlider;

namespace mn {

namespace gl {

struct Widget;

}

namespace ui {

struct GLPane : public QWidget {
	GLPane(std::auto_ptr<gl::AbstractObjects> theObjects,
	       gl::Configuration theConfig);

	gl::Widget *gl;

private:
	QSlider *hslider, *vslider;

	Q_OBJECT
};

}

}

#endif
