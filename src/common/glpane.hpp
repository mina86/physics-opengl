#ifndef H_GLPANE_HPP
#define H_GLPANE_HPP

#include <QWidget>

#include "config.hpp"

struct QSlider;

namespace mn {

namespace gl {

struct Widget;
struct AbstractScene;

}

namespace ui {

struct GLPane : public QWidget {
	GLPane(gl::Configuration theConfig, QWidget *parent = NULL);

	gl::Widget *gl;

private:
	QSlider *hslider, *vslider;

	Q_OBJECT
};

}

}

#endif
