#include <QGridLayout>
#include <QSlider>

#include "glpane.hpp"
#include "glwidget.hpp"
#include "config.hpp"

namespace mn {

namespace ui {

GLPane::GLPane(std::auto_ptr<gl::AbstractObjects> theObjects,
               gl::Configuration theConfig) {
	gl = new gl::Widget(theConfig);
	gl->setObjects(theObjects);

	vslider = new QSlider(Qt::Vertical);
	vslider->setSingleStep(gl::Widget::ticks_per_angle / 10);
	vslider->setPageStep(30 * gl::Widget::ticks_per_angle);
	vslider->setRange(-gl::Widget::vtick_limit, gl::Widget::vtick_limit);
	vslider->setSliderPosition(0);
	vslider->setTickPosition(QSlider::TicksBelow);
	vslider->setTickInterval(30 * gl::Widget::ticks_per_angle);

	hslider = new QSlider(Qt::Horizontal);
	hslider->setSingleStep(gl::Widget::ticks_per_angle / 10);
	hslider->setPageStep(30 * gl::Widget::ticks_per_angle);
	hslider->setRange(-180 * gl::Widget::ticks_per_angle,
	                  180 * gl::Widget::ticks_per_angle - 1);
	hslider->setSliderPosition(0);
	hslider->setTickPosition(QSlider::TicksRight);
	hslider->setTickInterval(30 * gl::Widget::ticks_per_angle);

	connect(gl, SIGNAL(vrotationChanged(int)),
	        vslider, SLOT(setValue(int)));
	connect(gl, SIGNAL(hrotationChanged(int)),
	        hslider, SLOT(setValue(int)));
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

}

}
