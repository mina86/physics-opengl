/*
 * src/ui/glpane.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
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
#include "glpane.hpp"

#include <QGridLayout>
#include <QSlider>

#include "../gl/glwidget.hpp"
#include "../gl/config.hpp"

namespace ui {

GLPane::GLPane(gl::Configuration theConfig, QWidget *parent)
	: QWidget(parent), gl(new gl::Widget(theConfig)) {
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

GLPane::~GLPane() {
	delete gl;
}

}
