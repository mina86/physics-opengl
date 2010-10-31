#ifndef H_GLPANE_HPP
#define H_GLPANE_HPP

#include <QWidget>

#include "config.hpp"

struct QSlider;

namespace mn {

namespace ui {

struct GLWidget;

struct GLPane : public QWidget {
	GLPane(GLWidget *theGL, QWidget *parent = NULL);

	GLWidget *gl;

public slots:
	void rotationChanged(int h, int v);

private:
	QSlider *hslider, *vslider;

	Q_OBJECT
};

}

}

#endif
