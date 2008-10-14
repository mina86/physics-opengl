#include <cmath>

#include "camera.hpp"

namespace mn {

void Camera::update() const {
	const float cx = std::cos(rotX), sx = std::sin(rotX);
	const float cy = std::cos(rotY), sy = std::sin(rotY);

	forward.y = cx;
	forward.x = sx * cy;
	forward.z = sx * sy;

	top.y = sx;
	top.x = -cx * cy;
	top.z = -cx * sy;

	left.y = 0;
	left.x = sy;
	left.z = -cy;

	valid = true;
}





Camera *Camera::camera = 0;
int Camera::wndWidth = 0, Camera::wndHeight = 0;
Camera::KeyboardFunc Camera::keyboardFunc = 0;
Camera::ResizeFunc Camera::resizeFunc = 0;
unsigned long Camera::mouseButtons = 0;
bool Camera::tickRedisplays = true, Camera::countTicks = true;
unsigned long Camera::ticks = 0;

float Camera::keyMovementFactor = 1.0;
float Camera::keyRotationTopFactor = 1.0;
float Camera::keyRotationLeftFactor = 1.0;
float Camera::mouseMovementFactor = 1.0;
float Camera::mouseRotationTopFactor = 1.0;
float Camera::mouseRotationLeftFactor = 1.0;


void Camera::handleKeyboardDown(unsigned char key, int x, int y) {
	(void)x; (void)y;
	if (camera) {
		bool postRedisplay = true;

		switch (key) {
		case 'a': camera->moveLeft( 0.1 * keyMovementFactor); break;
		case 'd': camera->moveLeft(-0.1 * keyMovementFactor); break;
		case 'w': camera->moveForward( 0.1 * keyMovementFactor); break;
		case 's': camera->moveForward(-0.1 * keyMovementFactor); break;
		case 't': camera->moveTop( 0.1 * keyMovementFactor); break;
		case 'g': camera->moveTop(-0.1 * keyMovementFactor); break;

		case 'q': camera->rotateLeft( 0.05 * keyRotationLeftFactor); break;
		case 'e': camera->rotateLeft(-0.05 * keyRotationLeftFactor); break;
		case 'r': camera->rotateTop( 0.05 * keyRotationTopFactor); break;
		case 'f': camera->rotateTop(-0.05 * keyRotationTopFactor); break;

		default: postRedisplay = false;
		}
		if (postRedisplay) {
			glutPostRedisplay();
		}
	}
	if (keyboardFunc) {
		keyboardFunc(key, true, x, y);
	}
}

void Camera::handleKeyboardUp(unsigned char key, int x, int y) {
	if (keyboardFunc) {
		keyboardFunc(key, false, x, y);
	}
}

void Camera::handleSpecialKeyboardDown(int key, int x, int y) {
	if (keyboardFunc) {
		keyboardFunc(key + 256, true, x, y);
	}
}

void Camera::handleSpecialKeyboardUp(int key, int x, int y) {
	if (keyboardFunc) {
		keyboardFunc(key + 256, false, x, y);
	}
}


void Camera::handleMouse(int button, int state, int x, int y) {
	(void)x; (void)y;
	if (button == 3 || button == 4) {
		if (camera && state == GLUT_DOWN) {
			camera->moveForward((button == 3 ? 0.5 : -0.5) * mouseMovementFactor);
			glutPostRedisplay();
		}
	} else if (state == GLUT_DOWN) {
		mouseButtons |= 1 << button;
	} else {
		mouseButtons &= ~(1 << button);
	}
}


void Camera::handleTick(int to) {
	if (countTicks) {
		++ticks;
	}
	bool forceRedisplay = false;
	if (camera && mouseButtons) {
		if (mouseButtons & (1 << GLUT_LEFT_BUTTON  )) camera->moveLeft( 0.1 * mouseMovementFactor);
		if (mouseButtons & (1 << GLUT_RIGHT_BUTTON )) camera->moveLeft(-0.1 * mouseMovementFactor);
		if (mouseButtons & (1 << GLUT_MIDDLE_BUTTON)) camera->moveForward(0.1 * mouseMovementFactor);
		forceRedisplay = true;
	}
	if (forceRedisplay || tickRedisplays) {
		glutPostRedisplay();
	}
	glutTimerFunc(to, handleTick, to);
}


void Camera::handleMotion(int x, int y) {
	static bool first = true;
	if (first) {
		first = false;
		return;
	}
	int dX = x - (wndWidth  >> 1);
	int dY = y - (wndHeight >> 1);
	if (dX || dY) {
		glutWarpPointer(wndWidth >> 1, wndHeight >> 1);
		if (camera) {
			camera->rotateTop(dY * 0.01 * mouseRotationTopFactor);
			camera->rotateLeft(-dX * 0.01 * mouseRotationLeftFactor);
			glutPostRedisplay();
		}
	}
}


void Camera::handleResize(int w, int h) {
	setSize(w, h);
	if (resizeFunc) {
		resizeFunc(w, h);
	}
}


void Camera::registerHandlers() {
	glutKeyboardFunc(handleKeyboardDown);
	glutKeyboardUpFunc(handleKeyboardUp);
	glutSpecialFunc(handleSpecialKeyboardDown);
	glutSpecialUpFunc(handleSpecialKeyboardUp);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutPassiveMotionFunc(handleMotion);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, handleTick, 25);
}


}
