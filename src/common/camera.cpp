#include "camera.hpp"

#include <stdio.h>

#include <cmath>

#include "mconst.h"

namespace mn {

namespace gl {


const float Camera::ID[16] = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};


Camera *Camera::camera = 0;
Camera::KeyboardFunc Camera::keyboardFunc = 0;
enum Camera::TickRedisplayPolicy Camera::tickRedisplayPolicy = Camera::WHEN_COUNTING;
bool Camera::nextTickRedisplays = true, Camera::countTicks = true;
unsigned long Camera::ticks = 0, Camera::tickIncrement = 10;

float Camera::keyMovementFactor       =  .1;
float Camera::keyRotationTopFactor    = MN_PI_180;
float Camera::keyRotationLeftFactor   = MN_PI_180;
float Camera::mouseMovementFactor     =  .1;
float Camera::mouseRotationTopFactor  = MN_PI_180 * 0.1;
float Camera::mouseRotationLeftFactor = MN_PI_180 * 0.1;
float Camera::runFactor               = 10.0;
float Camera::creepFactor             = 0.1;
float Camera::maxDistance             = 1500.0;

static unsigned wndWidth = 0, wndHeight = 0;
float Camera::_aspect = 0;

struct CameraImpl {
	static float &aspect() { return Camera::_aspect; }
	static float &aspect(float v) { return Camera::_aspect = v; }
	static float &aspect(float w, float h) { return Camera::_aspect = w / h; }
};

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

	ROT_LEFT       = 1 << 10,
	ROT_RIGHT      = 1 << 11,
	ROT_TOP        = 1 << 12,
	ROT_BOTTOM     = 1 << 13,

	RUN_FLAG       = 1 << 14,
	CREEP_FLAG     = 1 << 15
};

static unsigned actionsMask = 0;




static void handleKeyboardUp(unsigned char key, int x, int y);
static void handleSpecialKeyboardDown(int key, int x, int y);
static void handleSpecialKeyboardUp(int key, int x, int y);
static void handleMouse(int button, int state, int x, int y);
static void handleTick(int to);
static void handleMotion(int x, int y);



void Camera::update() const {
	const float cx =  std::cos(rotX), sx =  std::sin(rotX);
	const float cy = -std::cos(rotY), sy =  std::sin(rotY);

#define M(row, col) matrix[col * 4 + row]
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
#undef M

	valid = true;
}




void handleKeyboardDown(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': case 'R': actionsMask |= MOVE_FORWARD ; break;
	case 'f': case 'F': actionsMask |= MOVE_BACKWARD; break;
	case 'd': case 'D': actionsMask |= MOVE_LEFT    ; break;
	case 'g': case 'G': actionsMask |= MOVE_RIGHT   ; break;
	case 'w': case 'W': actionsMask |= MOVE_TOP     ; break;
	case 's': case 'S': actionsMask |= MOVE_BOTTOM  ; break;

	case 'e': case 'E': actionsMask |= ROT_LEFT     ; break;
	case 't': case 'T': actionsMask |= ROT_RIGHT    ; break;
	case 'y': case 'Y': actionsMask |= ROT_TOP      ; break;
	case 'h': case 'H': actionsMask |= ROT_BOTTOM   ; break;

	case 'z': case 'Z': actionsMask |= RUN_FLAG     ; break;
	case 'a': case 'A': actionsMask |= CREEP_FLAG   ; break;

	case 'q': case 'Q':
		if (Camera::camera) {
			Camera::camera->reset() ;
			Camera::nextTickRedisplays = true;
		}
		break;

	case ' ':
		Camera::countTicks = !Camera::countTicks;
		Camera::nextTickRedisplays = true;
		break;

	case '-':
		if (Camera::tickIncrement > 1) {
			--Camera::tickIncrement;
		}
		break;
	case '_':
		if (Camera::tickIncrement > 10) {
			Camera::tickIncrement -= 10;
		} else {
			Camera::tickIncrement = 1;
		}
		break;
	case '=':
		++Camera::tickIncrement;
		break;
	case '+':
		Camera::tickIncrement += 10;
		break;
	}
	if (Camera::keyboardFunc) {
		Camera::keyboardFunc(key, true, x, y);
	}
}

void handleKeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'r': case 'R': actionsMask &= ~MOVE_FORWARD ; break;
	case 'f': case 'F': actionsMask &= ~MOVE_BACKWARD; break;
	case 'd': case 'D': actionsMask &= ~MOVE_LEFT    ; break;
	case 'g': case 'G': actionsMask &= ~MOVE_RIGHT   ; break;
	case 'w': case 'W': actionsMask &= ~MOVE_TOP     ; break;
	case 's': case 'S': actionsMask &= ~MOVE_BOTTOM  ; break;

	case 'e': case 'E': actionsMask &= ~ROT_LEFT     ; break;
	case 't': case 'T': actionsMask &= ~ROT_RIGHT    ; break;
	case 'y': case 'Y': actionsMask &= ~ROT_TOP      ; break;
	case 'h': case 'H': actionsMask &= ~ROT_BOTTOM   ; break;

	case 'z': case 'Z': actionsMask &= ~RUN_FLAG     ; break;
	case 'a': case 'A': actionsMask &= ~CREEP_FLAG   ; break;
	}
	if (Camera::keyboardFunc) {
		Camera::keyboardFunc(key, false, x, y);
	}
}

void handleSpecialKeyboardDown(int key, int x, int y) {
	if (Camera::keyboardFunc) {
		Camera::keyboardFunc(key + 256, true, x, y);
	}
}

void handleSpecialKeyboardUp(int key, int x, int y) {
	if (Camera::keyboardFunc) {
		Camera::keyboardFunc(key + 256, false, x, y);
	}
}


void handleMouse(int button, int state, int x, int y) {
	(void)x; (void)y;
	if (button == 3 || button == 4) {
		if (state == GLUT_DOWN && Camera::camera) {
			Camera::camera->moveZ((button == 3 ? -5 : 5) *
			                      Camera::mouseMovementFactor);
			Camera::nextTickRedisplays = true;
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


void handleTick(int to) {
	if (Camera::countTicks) {
		Camera::ticks += Camera::tickIncrement;
	}
	if (Camera::camera && (actionsMask & ~(RUN_FLAG | CREEP_FLAG))) {
		const float speed = actionsMask & RUN_FLAG ? Camera::runFactor :
			(actionsMask & CREEP_FLAG ? Camera::creepFactor : 1.0);
#define IF(mask) if (actionsMask & (mask)) Camera::camera->
		IF(MOVE_FORWARD)  moveZ(-Camera::keyMovementFactor   * speed);
		IF(MOVE_BACKWARD) moveZ( Camera::keyMovementFactor   * speed);
		IF(MOVE_LEFT)     moveX(-Camera::keyMovementFactor   * speed);
		IF(MOVE_RIGHT)    moveX( Camera::keyMovementFactor   * speed);
		IF(MOVE_TOP)      moveY( Camera::keyMovementFactor   * speed);
		IF(MOVE_BOTTOM)   moveY(-Camera::keyMovementFactor   * speed);

		IF(MOUSE_FORWARD) moveZ(-Camera::mouseMovementFactor * speed);
		/* MOUSE_BACKWARD unused */
		IF(MOUSE_LEFT)    moveX(-Camera::mouseMovementFactor * speed);
		IF(MOUSE_RIGHT)   moveX( Camera::mouseMovementFactor * speed);

		IF(ROT_LEFT)      rotateLeft ( Camera::keyRotationLeftFactor);
		IF(ROT_RIGHT)     rotateLeft (-Camera::keyRotationLeftFactor);
		IF(ROT_TOP)       rotateTop  ( Camera::keyRotationTopFactor);
		IF(ROT_BOTTOM)    rotateTop  (-Camera::keyRotationTopFactor);
#undef IF
		goto redisplay;
	}

	if (Camera::nextTickRedisplays ||
	    Camera::tickRedisplayPolicy == Camera::ALWAYS ||
	    (Camera::tickRedisplayPolicy == Camera::WHEN_COUNTING &&
	     Camera::countTicks)){
	redisplay:
		glutPostRedisplay();
		Camera::nextTickRedisplays = false;
	}

	glutTimerFunc(to, handleTick, to);
}


void handleMotion(int x, int y) {
	static bool first = true;
	if (first) {
		first = false;
		return;
	}
	int dX = x - (wndWidth  >> 1);
	int dY = y - (wndHeight >> 1);
	if (dX || dY) {
		glutWarpPointer(wndWidth >> 1, wndHeight >> 1);
		if (Camera::camera) {
			Camera::camera->rotateTop ( dY * Camera::mouseRotationTopFactor);
			Camera::camera->rotateLeft(-dX * Camera::mouseRotationLeftFactor);
			Camera::nextTickRedisplays = true;
		}
	}
}


void handleResize(int w, int h) {
	wndWidth = w;
	wndHeight = h;
	glutWarpPointer(wndWidth >> 1, wndHeight >> 1);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, CameraImpl::aspect(w, h), 0.01, 3000.0);
}


void Camera::registerHandlers() {
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(handleKeyboardDown);
	glutKeyboardUpFunc(handleKeyboardUp);
	glutSpecialFunc(handleSpecialKeyboardDown);
	glutSpecialUpFunc(handleSpecialKeyboardUp);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutPassiveMotionFunc(handleMotion);
	glutReshapeFunc(handleResize);
	glutTimerFunc(20, handleTick, 20);
}



void Camera::printHelp() {
	puts("d/g  strafe right/left   e/d  move forward/backward    w/s  move up/dow\n"
		 "e/t  look   right/left   z/a  hold to move fast/slow   y/h  look up/down\n"
		 "q    reset position      -/=  inc./dec. speed by 1     spc  pause\n"
		 "esc  quit                _/+  inc./dec. speed by 10");
}

}

}
