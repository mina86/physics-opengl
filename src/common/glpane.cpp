#include <QGridLayout>
#include <QSlider>

#include "glpane.hpp"
#include "glwidget.hpp"
#include "config.hpp"

namespace mn {

namespace ui {

GLPane::GLPane(GLWidgetFactory &factory, QWidget *parent)
	: QWidget(parent) {
	gl = factory.create();

	vslider = new QSlider(Qt::Vertical);
	vslider->setSingleStep(GLWidget::ticks_per_angle);
	vslider->setPageStep(30 * GLWidget::ticks_per_angle);
	vslider->setRange(-GLWidget::vtick_limit, GLWidget::vtick_limit);
	vslider->setSliderPosition(0);
	vslider->setTickPosition(QSlider::TicksBelow);
	vslider->setTickInterval(30 * GLWidget::ticks_per_angle);

	hslider = new QSlider(Qt::Horizontal);
	hslider->setSingleStep(GLWidget::ticks_per_angle);
	hslider->setPageStep(30 * GLWidget::ticks_per_angle);
	hslider->setRange(-180 * GLWidget::ticks_per_angle,
	                  180 * GLWidget::ticks_per_angle - 1);
	hslider->setSliderPosition(0);
	hslider->setTickPosition(QSlider::TicksRight);
	hslider->setTickInterval(30 * GLWidget::ticks_per_angle);

	connect(gl, SIGNAL(rotationChanged(int, int)),
	        this, SLOT(rotationChanged(int, int)));
	connect(vslider, SIGNAL(valueChanged(int)),
	        gl, SLOT(vrotation(int)));
	connect(hslider, SIGNAL(valueChanged(int)),
	        gl, SLOT(hrotation(int)));

	QGridLayout *layout = new QGridLayout();
	layout->addWidget(gl, 0, 0);
	layout->addWidget(vslider, 0, 1);
	layout->addWidget(hslider, 1, 0);
	setLayout(layout);
}

void GLPane::rotationChanged(int h, int v) {
	if (h < 180 * GLWidget::ticks_per_angle) {
		hslider->setSliderPosition(h);
	} else {
		hslider->setSliderPosition(h - 360 * GLWidget::ticks_per_angle);
	}
	vslider->setSliderPosition(v);
}

}

}
