/*
 * src/gl/glwidget.cpp
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
#include "glwidget.hpp"

#include <cstring>

#include <Qt/QtGui>

#include "../lib/mconst.h"
#include "../lib/sintable.hpp"

#include "texture.hpp"

template<class T>
static inline T min(T a, T b) {
	return a < b ? a : b;
}

template<class T>
static inline T max(T a, T b) {
	return a < b ? b : a;
}


namespace gl {

const Widget::value_type Widget::zeros[4] = { 0, 0, 0, 1 };
const Widget::value_type Widget::ones[4] = { 1, 1, 1, 1 };
const Widget::value_type Widget::specular[4] = { 0.75, 0.75, 0.75, 1 };


/********************************** Misc ************************************/

QSize Widget::minimumSizeHint() const {
	return QSize(50, 50);
}

QSize Widget::sizeHint() const {
	return QSize(400, 400);
}

void Widget::setScene(AbstractScene::ptr theScene) {
	scene = theScene;
	emit sceneChanged();
	emit needRepaint();
}

/**************************** Camera & Rotation *****************************/

void Widget::camera(value_type x, value_type y, value_type z) {
	cam.x() = x;
	cam.y() = y;
	cam.z() = z;

	const value_type length = cam.length();
	if (length > MAX_DISTANCE) {
		cam *= MAX_DISTANCE / length;
	}

	emit needRepaint();
	emit cameraChanged(cam);
}

void Widget::camera(const Vector &v) {
	cam = v;

	const value_type length = cam.length();
	if (length > MAX_DISTANCE) {
		cam *= MAX_DISTANCE / length;
	}

	emit needRepaint();
	emit cameraChanged(cam);
}

void Widget::move(value_type x, value_type y, value_type z) {
	updateMatrix();
	/* M(0, 1) is always zero, see doUpdateMatrix(). */
	/*
	 * Also, let top-down movement change only y.  And finally, let
	 * left-right and forward-backward movement affect only x and z.
	 */
	camera(cam.x() + x * M(0, 0) +             z * M(2, 0),
	       cam.y() +               y * M(1, 1)            ,
	       cam.z() + x * M(0, 2) +             z * M(2, 2));
}

void Widget::rotation(int h, int v) {
	if (v < -vtick_limit) {
		v = -vtick_limit;
	} else if (v > vtick_limit) {
		v = vtick_limit;
	}

	h %= 360 * ticks_per_angle;

	if (v != rotX || h != rotY) {
		rotX = v;
		rotY = h;
		matrixValid = false;
		emit needRepaint();
		emit hrotationChanged(rotY >= 180 * ticks_per_angle
		                    ? rotY - 360 * ticks_per_angle : rotY);
		emit vrotationChanged(rotX);
	}
}

static const Widget::value_type ID[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

void Widget::reset() {
	rotX = 0;
	rotY = 0;
	std::memcpy(matrix, ID, sizeof matrix);
	matrixValid = true;
	/* camera() will emit needRepaint() */
	camera(Vector());
	emit hrotationChanged(0);
	emit vrotationChanged(0);
}

void Widget::doUpdateMatrix() const {
	if (!rotX && !rotY) {
		memcpy(matrix, ID, sizeof matrix);
	} else {
		const value_type _rotX = rotX * M_PI_180 / ticks_per_angle;
		const value_type _rotY = rotY * M_PI_180 / ticks_per_angle;
		const value_type cx =  std::cos(_rotX), sx =  std::sin(_rotX);
		const value_type cy = -std::cos(_rotY), sy =  std::sin(_rotY);

		/* x: right */
		M(0, 0) = -cy;
		/*M(0, 1) =   0;*/
		M(0, 2) =  sy;

		/* y: top */
		M(1, 0) = -sx * sy;
		M(1, 1) =  cx;
		M(1, 2) = -sx * cy;

		/* z: backward */
		M(2, 0) = -cx * sy;
		M(2, 1) = -sx;
		M(2, 2) = -cx * cy;
	}

	matrixValid = true;
}

bool Widget::isInFront(const Vector &vec) const {
	const value_type md1 =
		cam.x() * M(2, 0) + cam.y() * M(2, 1) + cam.z() * M(2, 2);
	const value_type md2 =
		vec.x() * M(2, 0) + vec.y() * M(2, 1) + vec.z() * M(2, 2);
	return md2 < md1;
}


/********************************** OpenGL **********************************/

Widget::Widget(Config &theConfig, QWidget *parent)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent), config(theConfig) {
	reset();
	connect(&config, SIGNAL(changed()), this, SLOT(configChanged()));
}

void Widget::configChanged() {
	emit needRepaint();
}

void Widget::doRotate() const {
	updateMatrix();
	glMultMatrixf(matrix);
}

void Widget::doMove() const {
	glTranslatef(-getX(), -getY(), -getZ());
}

void Widget::doLookAt() const {
	doRotate();
	doMove();
}

void Widget::initializeGL() {
	glClearColor(0.6, 0.6, 0.6, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_MULTISAMPLE);
}

void Widget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);

	_aspect = (value_type)width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, _aspect, 0.01, 3000.0);
}

void Widget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	/* Stars and move and rotate camera */
	glLoadIdentity();

	if (scene.get()) {
		scene->initializeGL(*this);
	}

	doRotate();
	paintStars();
	doMove();

	/* Draw the scene */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	if (scene.get()) {
		scene->drawScene(*this);
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	/* Draw helper axis in lower left */
	glLoadIdentity();
	glTranslatef(-0.4 * aspect(), -0.4, -1.2);
	doRotate();

	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.05, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.05, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.05);
	glEnd();
}

void Widget::paintStars() {
	if (!config->showStars) {
		return;
	}

	GLuint id = Texture::stars();
	if (!id) {
		return;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, id);
	glColor3f(1, 1, 1);

	const unsigned slices_deg = 60;
	const unsigned stacks_deg = 60;
	const float texMul = 1 / 120.0f;

	for (unsigned i = 0; i < 180; i += stacks_deg) {
		const float cos1 = lib::cos(i);
		const float sin1 = lib::sin(i);
		const float cos2 = lib::cos(i + stacks_deg);
		const float sin2 = lib::sin(i + stacks_deg);

		glBegin(GL_QUAD_STRIP);
		for (unsigned j = 0; j <= 360; j += slices_deg) {
			glTexCoord2f(j * texMul, i * texMul);
			glVertex3f(sin1 * lib::sin(j), sin1 * lib::cos(j), cos1);
			glTexCoord2f(j * texMul, (i + stacks_deg) * texMul);
			glVertex3f(sin2 * lib::sin(j), sin2 * lib::cos(j), cos2);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}


/***************************** OpenGL Utilities *****************************/

static void setColor(const float *color) {
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Widget::specular);
	glMaterialfv(GL_FRONT, GL_EMISSION, Widget::zeros);
	glMaterialf(GL_FRONT, GL_SHININESS, 12);
}

void Widget::_text(const std::string &, value_type,
                   const value_type[4]) const {
	/* XXX TODO: implement text */
}

void Widget::text(const std::string &, value_type,
                  const value_type[4]) const {
	/* XXX TODO: implement text */
}

bool Widget::sphere(value_type size, const Vector &point,
                    const value_type color[4],
                    const Texture *texture) const {
	value_type factor = cam.distance2(point);
	factor = factor > config->cutOffDistance2
		? std::sqrt(config->cutOffDistance2 / factor)
		: 1;

	unsigned slices = 60 * factor;
	if (size > 1) {
		slices <<= 1;
	}
	if (config->lowQuality) {
		slices >>= 2;
	}
	if (slices < 6) {
		slices = 6;
	}

	if (color) {
		setColor(color);
	}

	const bool gotTexture = config->showTextures && texture && texture->get();
	if (gotTexture) {
		glEnable(GL_TEXTURE_2D);
		gluQuadricTexture(config->quad, 1);
		glBindTexture(GL_TEXTURE_2D, texture->get());
		glPushMatrix();
		glRotatef(90, 1, 0, 0);
	}

	gluSphere(config->quad, size, slices, slices);

	if (gotTexture) {
		glPopMatrix();
		gluQuadricTexture(config->quad, 0);
		glDisable(GL_TEXTURE_2D);
	}

	if (config->showHelperAxis) {
		glPushMatrix();

		glDisable(GL_LIGHTING);

		doRotate();

		glLoadIdentity();
		glTranslatef(-point.x(), -point.y(), -point.z());

		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		// glVertex3f(-size - 2.0, 0, 0); glVertex3f(size + 2.0, 0, 0);
		// glColor3f(0, 1, 0);
		// glVertex3f(0, -size - 2.0, 0); glVertex3f(0, size + 2.0, 0);
		// glColor3f(0, 0, 1);
		// glVertex3f(0, 0, -size -2.0); glVertex3f(0, 0, size + 2.0);

		Vector t = top() * (size * (value_type)1.5);
		glVertex3f(-t.x() , -t.y(), -t.z()); glVertex3fv(t.v());

		glEnd();

		glEnable(GL_LIGHTING);

		glPopMatrix();
	}

	return factor < 0.99;
}

bool Widget::sphere(value_type size, const Vector &point,
                      const value_type color[4], const Texture *texture,
                      const std::string &) const {
	return sphere(size, point, color, texture);
	/* XXX TODO: implement text */
}

void Widget::connection(value_type size, Vector r,
                        const value_type color1[3],
                        const value_type color2[3]) const {
	value_type l = r.length();

	/* Don't draw anything if the points are too near */
	/* XXX hardcoded */
	if (l < 1.0) {
		return;
	}

	/* Make sure colors are given */
	color1 = color1 ? color1 : ones;
	color2 = color2 ? color2 : color1;

	/* If drawing style is anything else then fill, just draw a single
	 * line. */
	if (config->drawStyle) {
		glBegin(GL_LINES);
		glColor3fv(color1);
		glVertex3fv(zeros);
		glColor3fv(color2);
		glVertex3fv(r.v());
		glEnd();
		return;
	}

	/* Find two vectors on a plane orthogonal to r */
	Vector a, b;
	r /= l;
	if (fabs(r.x() - 1.0) < 0.0001) {
		a.set(0.0, 1.0, 0.0);
		b.set(0.0, 0.0, 1.0);
	} else {
		/* no square root calculation, yep! */
		float xm1 = r.x() - 1.0;
		a.set(r.z(), r.y() * r.z() / xm1, 1.0 + r.z() * r.z() / xm1);
		/* this probably could be calculated in a better way */
		b = r * a;
	}

	/* Calculate slices count */
	unsigned slices = 30;
	unsigned steps  = l * 10;
	if (config->lowQuality) {
		slices >>= 2;
		steps  >>= 2;
	}
	if (slices < 6) {
		slices = 6;
	}
	if (steps < 1) {
		steps = 1;
	}

	/* Let's get going ... */
	float step = l / steps;
	for (unsigned i = 0; i < steps; ++i) {
		Vector from = r * (i * step), to = r * ((i + 1) * step);

		glBegin(GL_QUAD_STRIP);
		for (unsigned j = 0; j <= slices; ++j) {
			Vector v =
				lib::sin(360 * j / slices) * a +
				lib::cos(360 * j / slices) * b;

			glNormal3fv(v.v());

			glColor3fv(color1);
			glVertex3f(from.x() + v.x() * size, from.y() + v.y() * size,
			           from.z() + v.z() * size);

			glColor3fv(color2);
			glVertex3f(to.x() + v.x() * size, to.y() + v.y() * size,
			           to.z() + v.z() * size);
		}
		glEnd();
	}
}

/********************************** Events **********************************/

void Widget::keyPressEvent(QKeyEvent *event)
{
	fprintf(stderr, "down %u\n", event->key());
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
	fprintf(stderr, "up %u\n", event->key());
}

void Widget::mousePressEvent(QMouseEvent *event) {
	lastPos = event->pos();
}

void Widget::mouseMoveEvent(QMouseEvent *event) {
	if (!(event->buttons() & (Qt::LeftButton | Qt::RightButton))) {
		return;
	}

	value_type factor;
	if (event->modifiers() & Qt::ShiftModifier) {
		factor = config->creepFactor;
	} else if (event->modifiers() & Qt::ControlModifier) {
		factor = config->runFactor;
	} else {
		factor = 1.0;
	}

	int dx = event->x() - lastPos.x(), dy = lastPos.y() - event->y();

	if (event->modifiers() & Qt::AltModifier) {
		factor *= config->mouseRotationFactor;
		rotate(dx * factor, dy * factor);
	} else /* !Alt */ {
		factor *= config->mouseMovementFactor;
		if (event->buttons() & Qt::LeftButton) {
			move(-dx * factor, -dy * factor, 0.0);
		} else /* RightButton */ {
			move(-dx * factor, 0.0, dy * factor);
		}
	}

	lastPos = event->pos();
}

}
