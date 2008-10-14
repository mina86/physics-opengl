#include <stdio.h>

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
bool Camera::tickRedisplays = true, Camera::countTicks = true;
unsigned long Camera::ticks = 0;

float Camera::keyMovementFactor       =  .1;
float Camera::keyRunFactor            =  .5;
float Camera::keyRotationTopFactor    = M_PI / 180;
float Camera::keyRotationLeftFactor   = M_PI / 180;
float Camera::mouseMovementFactor     =  .1;
float Camera::mouseRunFactor          =  .5;
float Camera::mouseRotationTopFactor  = M_PI / 1800;
float Camera::mouseRotationLeftFactor = M_PI / 1800;


static unsigned long actionsMask = 0;
enum {
	MOVE_FORWARD   = 1 <<  0,
	MOVE_BACKWARD  = 1 <<  1,
	MOVE_LEFT      = 1 <<  2,
	MOVE_RIGHT     = 1 <<  3,
	MOVE_TOP       = 1 <<  4,
	MOVE_BOTTOM    = 1 <<  5,

	MOUSE_FORWARD  = 1 <<  6,
	MOUSE_BACKWARD = 1 <<  7,  /* unused */
	MOUSE_LEFT     = 1 <<  8,
	MOUSE_RIGHT    = 1 <<  9,

	RUN_FORWARD    = 1 << 10,
	RUN_BACKWARD   = 1 << 11,
	RUN_LEFT       = 1 << 12,
	RUN_RIGHT      = 1 << 13,
	RUN_TOP        = 1 << 14,
	RUN_BOTTOM     = 1 << 15,

	ROT_LEFT       = 1 << 16,
	ROT_RIGHT      = 1 << 17,
	ROT_TOP        = 1 << 18,
	ROT_BOTTOM     = 1 << 19
};



void Camera::handleKeyboardDown(unsigned char key, int x, int y) {
	(void)x; (void)y;
	switch (key) {
	case 'w': actionsMask |= MOVE_FORWARD ; break;
	case 's': actionsMask |= MOVE_BACKWARD; break;
	case 'a': actionsMask |= MOVE_LEFT    ; break;
	case 'd': actionsMask |= MOVE_RIGHT   ; break;
	case 't': actionsMask |= MOVE_TOP     ; break;
	case 'g': actionsMask |= MOVE_BOTTOM  ; break;

	case 'W': actionsMask |= RUN_FORWARD ; break;
	case 'S': actionsMask |= RUN_BACKWARD; break;
	case 'A': actionsMask |= RUN_LEFT    ; break;
	case 'D': actionsMask |= RUN_RIGHT   ; break;
	case 'T': actionsMask |= RUN_TOP     ; break;
	case 'G': actionsMask |= RUN_BOTTOM  ; break;

	case 'Q': case 'q': actionsMask |= ROT_LEFT  ; break;
	case 'E': case 'e': actionsMask |= ROT_RIGHT ; break;
	case 'R': case 'r': actionsMask |= ROT_TOP   ; break;
	case 'F': case 'f': actionsMask |= ROT_BOTTOM; break;
	}
	if (keyboardFunc) {
		keyboardFunc(key, true, x, y);
	}
}

void Camera::handleKeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w': actionsMask &= ~MOVE_FORWARD ; break;
	case 's': actionsMask &= ~MOVE_BACKWARD; break;
	case 'a': actionsMask &= ~MOVE_LEFT    ; break;
	case 'd': actionsMask &= ~MOVE_RIGHT   ; break;
	case 't': actionsMask &= ~MOVE_TOP     ; break;
	case 'g': actionsMask &= ~MOVE_BOTTOM  ; break;

	case 'W': actionsMask &= ~RUN_FORWARD ; break;
	case 'S': actionsMask &= ~RUN_BACKWARD; break;
	case 'A': actionsMask &= ~RUN_LEFT    ; break;
	case 'D': actionsMask &= ~RUN_RIGHT   ; break;
	case 'T': actionsMask &= ~RUN_TOP     ; break;
	case 'G': actionsMask &= ~RUN_BOTTOM  ; break;

	case 'Q': case 'q': actionsMask &= ~ROT_LEFT  ; break;
	case 'E': case 'e': actionsMask &= ~ROT_RIGHT ; break;
	case 'R': case 'r': actionsMask &= ~ROT_TOP   ; break;
	case 'F': case 'f': actionsMask &= ~ROT_BOTTOM; break;
	}
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
		if (state == GLUT_DOWN) {
			camera->moveForward((button == 3 ? 5 : -5) * mouseMovementFactor);
			glutPostRedisplay();
		}
	} else if (state == GLUT_DOWN) {
		switch (button) {
		case GLUT_LEFT_BUTTON  : actionsMask |= MOUSE_LEFT   ; break;
		case GLUT_RIGHT_BUTTON : actionsMask |= MOUSE_RIGHT  ; break;
		case GLUT_MIDDLE_BUTTON: actionsMask |= MOUSE_FORWARD; break;
		}
	} else {
		switch (button) {
		case GLUT_LEFT_BUTTON  : actionsMask &= ~MOUSE_LEFT   ; break;
		case GLUT_RIGHT_BUTTON : actionsMask &= ~MOUSE_RIGHT  ; break;
		case GLUT_MIDDLE_BUTTON: actionsMask &= ~MOUSE_FORWARD; break;
		}
	}
}


void Camera::handleTick(int to) {
	if (countTicks) {
		++ticks;
	}
	if (tickRedisplays) {
		glutPostRedisplay();
	}
	if (camera && actionsMask) {
#define IF(mask) if (actionsMask & (mask))
		IF(MOVE_FORWARD)  camera->moveForward( keyMovementFactor);
		IF(MOVE_BACKWARD) camera->moveForward(-keyMovementFactor);
		IF(MOVE_LEFT)     camera->moveLeft   ( keyMovementFactor);
		IF(MOVE_RIGHT)    camera->moveLeft   (-keyMovementFactor);
		IF(MOVE_TOP)      camera->moveTop    ( keyMovementFactor);
		IF(MOVE_BOTTOM)   camera->moveTop    (-keyMovementFactor);

		IF(MOUSE_FORWARD) camera->moveForward( mouseMovementFactor);
		/* MOUSE_BACKWARD unused */
		IF(MOUSE_LEFT)     camera->moveLeft  ( mouseMovementFactor);
		IF(MOUSE_RIGHT)    camera->moveLeft  (-mouseMovementFactor);

		IF(RUN_FORWARD)   camera->moveForward( keyRunFactor);
		IF(RUN_BACKWARD)  camera->moveForward(-keyRunFactor);
		IF(RUN_LEFT)      camera->moveLeft   ( keyRunFactor);
		IF(RUN_RIGHT)     camera->moveLeft   (-keyRunFactor);
		IF(RUN_TOP)       camera->moveTop    ( keyRunFactor);
		IF(RUN_BOTTOM)    camera->moveTop    (-keyRunFactor);

		IF(ROT_LEFT)      camera->rotateLeft ( keyRotationLeftFactor);
		IF(ROT_RIGHT)     camera->rotateLeft (-keyRotationLeftFactor);
		IF(ROT_TOP)       camera->rotateTop  ( keyRotationTopFactor);
		IF(ROT_BOTTOM)    camera->rotateTop  (-keyRotationTopFactor);
#undef IF
	}

	if (tickRedisplays || actionsMask) {
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
			camera->rotateTop(dY   * mouseRotationTopFactor);
			camera->rotateLeft(-dX * mouseRotationLeftFactor);
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
	glutTimerFunc(40, handleTick, 40);
}


}
