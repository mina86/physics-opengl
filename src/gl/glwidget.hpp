/*
 * src/gl/glwidget.hpp
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
#ifndef H_GLWIDGET_HPP
#define H_GLWIDGET_HPP

#include <memory>

#include <QtOpenGL/QGLWidget>

#include "../lib/mconst.h"

#include "vector.hpp"
#include "glconfig.hpp"
#include "abstract-scene.hpp"

namespace gl {

struct Texture;

struct Widget : public QGLWidget {
	typedef gl::Vector<float> Vector;
	typedef Vector::value_type value_type;

	static const value_type zeros[4], ones[4], specular[4];

	static const int ticks_per_angle = 60;
	static const int vtick_limit = ticks_per_angle * 90 - 1;

	static value_type rad_from_ticks(int ticks) {
		return ticks * M_PI_180 / ticks_per_angle;
	}
	static int ticks_from_rad(float rad) {
		return round(rad * M_180_PI * ticks_per_angle);
	}

	Widget(Config &theConfig, QWidget *parent = NULL);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

	const Vector &camera() const { return cam; }
	value_type getX() const { return cam.x(); }
	value_type getY() const { return cam.y(); }
	value_type getZ() const { return cam.z(); }

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
	void camera(const Vector &v);
	void move(value_type x, value_type y, value_type z);

	void rotation(int h, int v);
	void rotate(int h, int v) {
		rotation(rotY + h, rotX + v);
	}

	void doRotate() const;
	void doMove() const;
	void doLookAt() const;

	bool sphere(value_type size, const Vector &point,
	            const value_type color[4] = NULL,
	            const Texture *texture = NULL) const;
	bool sphere(value_type size, const Vector &point,
	            const Texture *texture = NULL) const {
		return sphere(size, point, NULL, texture);
	}
	bool sphere(value_type size, const Vector &point,
	            const value_type color[4], const Texture *texture,
	            const std::string &text) const;
	bool sphere(value_type size, const Vector &point,
	            const value_type color[4], const std::string &text) const {
		return sphere(size, point, color, NULL, text);
	}
	bool sphere(value_type size, const Vector &point,
	            const Texture *texture, const std::string &text) const {
		return sphere(size, point, NULL, texture, text);
	}
	bool sphere(value_type size, const Vector &point,
	            const std::string &text) const {
		return sphere(size, point, NULL, NULL, text);
	}

	void text(const std::string &text, value_type scale,
	          const value_type color[4] = NULL) const;
	void _text(const std::string &text, value_type scale,
	           const value_type color[4] = NULL) const;

	void connection(value_type size, Vector r,
	                const value_type color1[3] = NULL,
	                const value_type color2[3] = NULL) const;

	bool isInFront(const Vector &v) const;

	void setScene(AbstractScene::ptr theScene);
	AbstractScene *getScene() {
		return scene.get();
	}
	template<class Scene>
	Scene *getScene() {
		return dynamic_cast<Scene *>(getScene());
	}

	const Config config;

public slots:
	virtual void updateState(unsigned ticks, float dt) {
		if (scene.get()) {
			scene->updateState(ticks, dt);
		}
	}

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
	void sceneChanged();
	void needRepaint();
	void hrotationChanged(int h);
	void vrotationChanged(int v);
	void cameraChanged(const Vector &camera);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	virtual void paintStars();

	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	Widget(const Widget &);
	Widget &operator=(const Widget &);

	AbstractScene::ptr scene;

	Vector cam;
	value_type _aspect;

	inline value_type &M(unsigned col, unsigned row) const {
		return matrix[col | (row << 2)];
	}
	void updateMatrix() const {
		if (!matrixValid) {
			doUpdateMatrix();
		}
	}
	void doUpdateMatrix() const;

	int rotX, rotY;
	mutable value_type matrix[16];
	mutable bool matrixValid;

	QPoint lastPos;

	Q_OBJECT
};

}

#endif
