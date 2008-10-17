#include <stdio.h>

#include <cmath>

#include "camera.hpp"

namespace mn {

namespace gl {



Camera *Camera::camera = 0;
Camera::KeyboardFunc Camera::keyboardFunc = 0;
bool Camera::tickRedisplays = true, Camera::countTicks = true;
unsigned long Camera::ticks = 0;

float Camera::keyMovementFactor       =  .1;
float Camera::keyRotationTopFactor    = M_PI / 180.0;
float Camera::keyRotationLeftFactor   = M_PI / 180.0;
float Camera::mouseMovementFactor     =  .1;
float Camera::mouseRotationTopFactor  = M_PI / 1800.0;
float Camera::mouseRotationLeftFactor = M_PI / 1800.0;
float Camera::runFactor               = 10.0;
float Camera::creepFactor             = 0.1;
float Camera::maxDistance             = 1500.0;

unsigned Camera::wndWidth = 0, Camera::wndHeight = 0;
unsigned Camera::viewport = 16;


struct CameraImpl {
	static inline unsigned &width   () { return Camera::wndWidth ; }
	static inline unsigned &height  () { return Camera::wndHeight; }
	static inline unsigned &viewport() { return Camera::viewport ; }
	static inline void doViewport(unsigned vp) { Camera::doViewport(vp); }
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


void Camera::doRotate() const {
	if (!valid) update();

	float matrix[16];
#define M(row, col) matrix[col * 4 + row]
	M(0, 0) = -left.x;
	M(0, 1) = -left.y;
	M(0, 2) = -left.z;
	M(0, 3) = 0.0;
	M(1, 0) = top.x;
	M(1, 1) = top.y;
	M(1, 2) = top.z;
	M(1, 3) = 0.0;
	M(2, 0) = -forward.x;
	M(2, 1) = -forward.y;
	M(2, 2) = -forward.z;
	M(2, 3) = 0.0;
	M(3, 0) = 0.0;
	M(3, 1) = 0.0;
	M(3, 2) = 0.0;
	M(3, 3) = 1.0;
#undef M
	glMultMatrixf(matrix);
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
			glutPostRedisplay();
		}
		break;

	case '-': case '_':
		if (CameraImpl::viewport() > 2) {
			CameraImpl::doViewport(--CameraImpl::viewport());
			glutPostRedisplay();
		}
		break;
	case '+': case '=':
		if (CameraImpl::viewport() < 16) {
			CameraImpl::doViewport(++CameraImpl::viewport());
			glutPostRedisplay();
		}
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
			Camera::camera->moveForward((button == 3 ? 5 : -5) *
			                            Camera::mouseMovementFactor);
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


void handleTick(int to) {
	if (Camera::countTicks) {
		++Camera::ticks;
	}
	if (Camera::camera && (actionsMask & ~(RUN_FLAG | CREEP_FLAG))) {
		const float speed = actionsMask & RUN_FLAG ? Camera::runFactor :
			(actionsMask & CREEP_FLAG ? Camera::creepFactor : 1.0);
#define IF(mask) if (actionsMask & (mask)) Camera::camera->
		IF(MOVE_FORWARD)  moveForward( Camera::keyMovementFactor   * speed);
		IF(MOVE_BACKWARD) moveForward(-Camera::keyMovementFactor   * speed);
		IF(MOVE_LEFT)     moveLeft   ( Camera::keyMovementFactor   * speed);
		IF(MOVE_RIGHT)    moveLeft   (-Camera::keyMovementFactor   * speed);
		IF(MOVE_TOP)      moveTop    ( Camera::keyMovementFactor   * speed);
		IF(MOVE_BOTTOM)   moveTop    (-Camera::keyMovementFactor   * speed);

		IF(MOUSE_FORWARD) moveForward( Camera::mouseMovementFactor * speed);
		/* MOUSE_BACKWARD unused */
		IF(MOUSE_LEFT)    moveLeft   ( Camera::mouseMovementFactor * speed);
		IF(MOUSE_RIGHT)   moveLeft   (-Camera::mouseMovementFactor * speed);

		IF(ROT_LEFT)      rotateLeft ( Camera::keyRotationLeftFactor);
		IF(ROT_RIGHT)     rotateLeft (-Camera::keyRotationLeftFactor);
		IF(ROT_TOP)       rotateTop  ( Camera::keyRotationTopFactor);
		IF(ROT_BOTTOM)    rotateTop  (-Camera::keyRotationTopFactor);
#undef IF
		goto redisplay;
	}

	if (Camera::tickRedisplays) {
	redisplay:
		glutPostRedisplay();
	}

	glutTimerFunc(to, handleTick, to);
}


void handleMotion(int x, int y) {
	static bool first = true;
	if (first) {
		first = false;
		return;
	}
	int dX = x - (CameraImpl::width () >> 1);
	int dY = y - (CameraImpl::height() >> 1);
	if (dX || dY) {
		glutWarpPointer(CameraImpl::width() >> 1, CameraImpl::height() >> 1);
		if (Camera::camera) {
			Camera::camera->rotateTop ( dY * Camera::mouseRotationTopFactor);
			Camera::camera->rotateLeft(-dX * Camera::mouseRotationLeftFactor);
			glutPostRedisplay();
		}
	}
}


void handleResize(int w, int h) {
	CameraImpl::width() = w;
	CameraImpl::height() = h;
	glutWarpPointer(CameraImpl::width() >> 1, CameraImpl::height() >> 1);
	CameraImpl::doViewport(CameraImpl::viewport());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 0.01, 10000.0);
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
		 "q    reset position      -/=  dec-/increase viewport   esc  quit");
}

}

}
