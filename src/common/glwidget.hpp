#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>

#include "vector.hpp"
#include "config.hpp"
#include "mconst.h"

namespace mn {

namespace gl {

struct Texture;

}

namespace ui {

struct GLWidget : public QGLWidget {
	typedef gl::Vector<float> Vector;
	typedef Vector::value_type value_type;

	static const value_type zeros[4], ones[4], specular[4];

	static const int ticks_per_angle = 60;
	static const int vtick_limit = ticks_per_angle * 90 - 1;

	static value_type rad_from_ticks(int ticks) {
		return ticks * MN_PI_180 / ticks_per_angle;
	}
	static int ticks_from_rad(float rad) {
		return round(rad * MN_180_PI * ticks_per_angle);
	}

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	const Vector &camera() const { return cam; }
	value_type getX() const { return cam.x; }
	value_type getY() const { return cam.y; }
	value_type getZ() const { return cam.z; }

	int hrotation() const { return rotY; }
	int vrotation() const { return rotY; }

	Vector forward() const {
		updateMatrix();
		return Vector(-M(2, 0), -M(2, 1), -M(2, 2));
	}
	Vector right() const {
		updateMatrix();
		return Vector(M(0, 0), M(0, 1), M(0, 1));
	}
	Vector top() const {
		updateMatrix();
		return Vector(M(1, 0), M(1, 1), M(1, 2));
	}

	value_type aspect() const { return _aspect; }

	void camera(value_type x, value_type y, value_type z);
	void camera(const Vector &v) { camera(v.x, v.y, v.z); }
	void move(value_type x, value_type y, value_type z);

	void rotation(int h, int v);
	void rotate(int h, int v) {
		rotation(rotY + h, rotX + v);
	}

	bool sphere(value_type size, const Vector &point,
	            const value_type color[4] = NULL,
	            const gl::Texture *texture = NULL) const;
	bool sphere(value_type size, const Vector &point,
	            const gl::Texture *texture = NULL) const {
		return sphere(size, point, NULL, texture);
	}
	bool sphere(value_type size, const Vector &point,
	            const value_type color[4],
	            const gl::Texture *texture,
	            const std::string &text, unsigned *list) const;
	bool sphere(value_type size, const Vector &point,
	            const value_type color[4],
	            const std::string &text, unsigned *list) const {
		return sphere(size, point, color, NULL, text, list);
	}
	bool sphere(value_type size, const Vector &point,
	            const gl::Texture *texture,
	            const std::string &text, unsigned *list) const {
		return sphere(size, point, NULL, texture, text, list);
	}
	bool sphere(value_type size, const Vector &point,
	            const std::string &text, unsigned *list) const {
		return sphere(size, point, NULL, NULL, text, list);
	}

	void text(const std::string &text, value_type scale, unsigned *list = NULL,
	          const value_type color[4] = NULL) const;
	void _text(const std::string &text, value_type scale, unsigned *list = NULL,
	           const value_type color[4] = NULL) const;

	bool isInFront(const Vector &v) const;

	unsigned getDrawStyle() const { return drawStyle; }

	void updateMatrix() const {
		if (!matrixValid) {
			doUpdateMatrix();
		}
	}

	const gl::Configuration::ptr config;

public slots:
	void setX(int x) { camera(x, getY(), getZ()); }
	void setY(int y) { camera(getX(), y, getZ()); }
	void setZ(int z) { camera(getX(), getY(), z); }

	void moveLeft(int x)    { move(x, 0, 0); }
	void moveTop(int y)     { move(0, y, 0); }
	void moveForward(int z) { move(0, 0, z); }

	void rotateVertically(int ticks) { rotate(0, ticks); }
	void rotateHorizontally(int ticks) { rotate(ticks, 0); }
	void vrotation(int ticks) { rotation(rotY, ticks); }
	void hrotation(int ticks) { rotation(ticks, rotX); }

	void reset();

private slots:
	void configChanged();

signals:
	void needRepaint();
	void rotationChanged(int h, int v);
	void cameraChanged(const Vector &camera);

protected:
	GLWidget(QWidget *parent, gl::Configuration &theConfig);

	void doRotate() const;
	void doMove() const;
	void doLookAt() const;

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	virtual void doPaint() = 0;

	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	GLWidget(const GLWidget &);
	GLWidget &operator=(const GLWidget &);

	Vector cam;
	value_type _aspect;

	inline value_type &M(unsigned col, unsigned row) const {
		return matrix[col | (row << 2)];
	}

	void doUpdateMatrix() const;

	int rotX, rotY;
	mutable value_type matrix[16];
	mutable bool matrixValid;

	QPoint lastPos;

	unsigned drawStyle;

	Q_OBJECT
};

}

}

#endif
