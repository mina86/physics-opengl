#include <QGridLayout>

#include "glpane.hpp"
#include "glwidget.hpp"
#include "config.hpp"

namespace mn {

namespace ui {

GLPane::GLPane(GLWidgetFactory &factory, QWidget *parent)
	: QWidget(parent) {
	gl = factory.create();

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(gl, 0, 0);
	setLayout(layout);
}

}

}
