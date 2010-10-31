#ifndef H_GLPANE_HPP
#define H_GLPANE_HPP

#include <QWidget>

#include "config.hpp"

struct QSlider;

namespace mn {

namespace ui {

struct GLWidget;

struct GLPane : public QWidget {
	struct GLWidgetFactory {
		GLWidgetFactory(gl::Configuration &theConfig)
			: config(theConfig) { }

		GLWidget *create(QWidget *parent = NULL) {
			return create(config, parent);
		}

	protected:
		virtual GLWidget *create(gl::Configuration &theConfig,
		                         QWidget *parent) = 0;

	private:
		gl::Configuration::ptr config;
	};

	GLPane(GLWidgetFactory &factory, QWidget *parent = NULL);

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
