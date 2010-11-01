#include <cstring>

#include <Qt/QtGui>

#include "glwidget.hpp"
#include "text3d.hpp"
#include "texture.hpp"
#include "mconst.h"
#include "sintable.hpp"

namespace mn {

namespace ui {

const GLWidget::value_type GLWidget::zeros[4] = { 0, 0, 0, 1 };
const GLWidget::value_type GLWidget::ones[4] = { 1, 1, 1, 1 };
const GLWidget::value_type GLWidget::specular[4] = { 0.75, 0.75, 0.75, 1 };


/********************************** Hints ***********************************/

QSize GLWidget::minimumSizeHint() const {
	return QSize(50, 50);
}

QSize GLWidget::sizeHint() const {
	return QSize(400, 400);
}


/**************************** Camera & Rotation *****************************/

void GLWidget::camera(value_type x, value_type y, value_type z) {
	cam.x = x;
	cam.y = y;
	cam.z = z;

	const value_type length = cam.length();
	if (length > MAX_DISTANCE) {
		cam *= MAX_DISTANCE / length;
	}

	emit needRepaint();
	emit cameraChanged(cam);
}

void GLWidget::move(value_type x, value_type y, value_type z) {
	updateMatrix();
	/* M(0, 1) is always zero, see doUpdateMatrix(). */
	/*
	 * Also, let top-down movement change only y.  And finally, let
	 * left-right and forward-backward movement affect only x and z.
	 */
	camera(cam.x + x * M(0, 0) +               z * M(2, 0),
	       cam.y +               y * M(1, 1)              ,
	       cam.z + x * M(0, 2) +               z * M(2, 2));
}

void GLWidget::rotation(int h, int v) {
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
		emit rotationChanged(rotY >= 180 * ticks_per_angle
		                   ? rotY - 360 * ticks_per_angle : rotY, rotX);
	}
}

static const GLWidget::value_type ID[16] = {
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0
};

void GLWidget::reset() {
	rotX = 0;
	rotY = 0;
	std::memcpy(matrix, ID, sizeof matrix);
	matrixValid = true;
	emit rotationChanged(0, 0);
	/* camera() will emit needRepaint() */
	camera(Vector());
}

void GLWidget::doUpdateMatrix() const {
	if (!rotX && !rotY) {
		memcpy(matrix, ID, sizeof matrix);
	} else {
		const value_type _rotX = rotX * MN_PI_180 / ticks_per_angle;
		const value_type _rotY = rotY * MN_PI_180 / ticks_per_angle;
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


bool GLWidget::isInFront(const Vector &vec) const {
	const value_type md1 = cam.x * M(0, 2) + cam.y * M(1, 2) + cam.z * M(2, 2);
	const value_type md2 = vec.x * M(0, 2) + vec.y * M(1, 2) + vec.z * M(2, 2);
	return md2 < md1;
}


/********************************** OpenGL **********************************/

GLWidget::GLWidget(QWidget *parent, gl::Configuration &theConfig)
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
	  config(theConfig) {
	reset();
	connect(&config, SIGNAL(changed()), this, SLOT(configChanged()));
}

void GLWidget::configChanged() {
	emit needRepaint();
}

void GLWidget::doRotate() const {
	updateMatrix();
	glMultMatrixf(matrix);
}

void GLWidget::doMove() const {
	glTranslatef(-getX(), -getY(), -getZ());
}

void GLWidget::doLookAt() const {
	doRotate();
	doMove();
}

void GLWidget::initializeGL() {
	glClearColor(0.6, 0.6, 0.6, 1.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_MULTISAMPLE);
}

void GLWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);

	_aspect = (value_type)width / height;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, _aspect, 0.01, 3000.0);
}

void GLWidget::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	/* Stars and move and rotate camera */
	glLoadIdentity();
	doRotate();
	paintStars();
	doMove();

	/* Draw the scene */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);

	doPaint();

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

void GLWidget::paintStars() {
	if (!config->showStars) {
		return;
	}

	GLuint id = gl::Texture::stars();
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
		const float cos1 = mn::cos(i);
		const float sin1 = mn::sin(i);
		const float cos2 = mn::cos(i + stacks_deg);
		const float sin2 = mn::sin(i + stacks_deg);

		glBegin(GL_QUAD_STRIP);
		for (unsigned j = 0; j <= 360; j += slices_deg) {
			glTexCoord2f(j * texMul, i * texMul);
			glVertex3f(sin1 * mn::sin(j), sin1 * mn::cos(j), cos1);
			glTexCoord2f(j * texMul, (i + stacks_deg) * texMul);
			glVertex3f(sin2 * mn::sin(j), sin2 * mn::cos(j), cos2);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);
}


/***************************** OpenGL Utilities *****************************/

void GLWidget::_text(const std::string &text, value_type scale, unsigned *list,
                     const value_type color[4]) const {
	if (config->showText && !text.empty()) {
		if (list && *list) {
			glCallList(*list);
			return;
		}

		if (list) {
			*list = glGenLists(1);
			if (*list) {
				glNewList(*list, GL_COMPILE_AND_EXECUTE);
			}
		}

		glScalef(scale, scale, scale);
		if (color) {
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		}
		glMaterialfv(GL_FRONT, GL_EMISSION, zeros);
		t3d::draw3D(text, 0, 0, 0.5);

		if (list && *list) {
			glEndList();
		}
	}
}

void GLWidget::text(const std::string &text, value_type scale, unsigned *list,
                    const value_type color[4]) const {
	if (config->showText && !text.empty()) {
		glPushMatrix();
		_text(text, scale, list, color);
		glPopMatrix();
	}
}

bool GLWidget::sphere(value_type size, const Vector &point,
                      const value_type color[4],
                      const gl::Texture *texture) const {
	value_type factor = cam.distance2(point);
	factor = factor > config->cutOffDistance2
		? std::sqrt(config->cutOffDistance2 / factor)
		: 1;

	unsigned slices = 60 * factor;
	if (size > 1) {
		slices *= 2;
	}
	if (config->lowQuality) {
		slices /= 3;
	}
	if (slices < 6) {
		slices = 6;
	}

	if (color) {
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, zeros);
		glMaterialf(GL_FRONT, GL_SHININESS, 12);
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
		glTranslatef(-point.x, -point.y, -point.z);

		glBegin(GL_LINES);
		glColor3f(1, 0, 0);
		// glVertex3f(-size - 2.0, 0, 0); glVertex3f(size + 2.0, 0, 0);
		// glColor3f(0, 1, 0);
		// glVertex3f(0, -size - 2.0, 0); glVertex3f(0, size + 2.0, 0);
		// glColor3f(0, 0, 1);
		// glVertex3f(0, 0, -size -2.0); glVertex3f(0, 0, size + 2.0);

		Vector t = top() * (size * (value_type)1.5);
		glVertex3f(-t.x , -t.y, -t.z); glVertex3f(t.x, t.y, t.z);

		glEnd();

		glEnable(GL_LIGHTING);

		glPopMatrix();
	}

	return factor < 0.99;
}

bool GLWidget::sphere(value_type size, const Vector &point,
                      const value_type color[4],
                      const gl::Texture *texture,
                      const std::string &text, unsigned *list) const {
	if (!sphere(size, point, texture)) {
		return false;
	}

	if (config->showText && text.empty()) {
		glPushMatrix();

		glLoadIdentity();

		Vector v(top());
		v *= 0.3;
		v += point;
		glTranslatef(-point.x, -point.y, -point.z);

		_text(text, 0.1, list, color);

		glPopMatrix();
	}

	return true;
}


/********************************** Events **********************************/

void GLWidget::keyPressEvent(QKeyEvent *event)
{
	fprintf(stderr, "down %u\n", event->key());
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
	fprintf(stderr, "up %u\n", event->key());
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
	lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
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

}
