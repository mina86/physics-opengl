#include <stdlib.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "types.hpp"


static mn::Camera camera;
static unsigned long mouseButtons = 0;
static unsigned long tick = 0;
static int wndWidth = 400, wndHeight = 400;
static struct { GLuint cube; } displayLists;



static void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 27:  /* Escape */
		exit(0);

	case 'a': camera.moveLeft( 0.1); break;
	case 'd': camera.moveLeft(-0.1); break;
	case 'w': camera.moveForward( 0.1); break;
	case 's': camera.moveForward(-0.1); break;
	case 't': camera.moveTop( 0.1); break;
	case 'g': camera.moveTop(-0.1); break;

	case 'q': camera.rotateLeft( 0.05); break;
	case 'e': camera.rotateLeft(-0.05); break;
	case 'r': camera.rotateTop( 0.05); break;
	case 'f': camera.rotateTop(-0.05); break;

	default:
		printf("%c [%d]\n", key, key);
		return;
	}
	glutPostRedisplay();
}


static void handleMouse(int button, int state, int x, int y) {
	if (button == 3 || button == 4) {
		if (state == GLUT_DOWN) {
			camera.moveForward(button == 3 ? 0.5 : -0.5);
			glutPostRedisplay();
		}
	} else if (state == GLUT_DOWN) {
		mouseButtons |= 1 << button;
	} else {
		mouseButtons &= ~(1 << button);
	}
}


static void handleTick(int to) {
	++tick;
	if (mouseButtons) {
		if (mouseButtons & (1 << GLUT_LEFT_BUTTON  )) camera.moveLeft( 0.1);
		if (mouseButtons & (1 << GLUT_RIGHT_BUTTON )) camera.moveLeft(-0.1);
		if (mouseButtons & (1 << GLUT_MIDDLE_BUTTON)) camera.moveForward(0.1);
	}
	glutPostRedisplay();
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
		camera.rotateTop(dY * 0.01);
		camera.rotateLeft(-dX * 0.01);
		glutPostRedisplay();
	}
}


static void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
	glutWarpPointer((wndWidth = w) >> 1, (wndHeight = h) >> 1);
}


static void drawCube() {
	glBegin(GL_QUAD_STRIP);
	glNormal3f(0, 0, 1);
	glColor4f(0.0, 0.0, 0.0, 0.75); glVertex3f(-1, -1,  1);
	glColor4f(0.0, 0.0, 1.0, 0.75); glVertex3f( 1, -1,  1);
	glColor4f(0.0, 1.0, 0.0, 0.75); glVertex3f(-1,  1,  1);
	glColor4f(0.0, 1.0, 1.0, 0.75); glVertex3f( 1,  1,  1);
	glNormal3f(0, 1, 0);
	glColor4f(1.0, 0.0, 0.0, 0.75); glVertex3f(-1,  1, -1);
	glColor4f(1.0, 0.0, 1.0, 0.75); glVertex3f( 1,  1, -1);
	glNormal3f(0, 0, -1);
	glColor4f(1.0, 1.0, 0.0, 0.75); glVertex3f(-1, -1, -1);
	glColor4f(1.0, 1.0, 1.0, 0.75); glVertex3f( 1, -1, -1);
	glNormal3f(0, -1, 0);
	glColor4f(0.0, 0.0, 0.0, 0.75); glVertex3f(-1, -1,  1);
	glColor4f(0.0, 0.0, 1.0, 0.75); glVertex3f( 1, -1,  1);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glColor4f(0.0, 0.0, 0.0, 0.75); glVertex3f(-1, -1,  1);
	glColor4f(0.0, 1.0, 0.0, 0.75); glVertex3f(-1,  1,  1);
	glColor4f(1.0, 0.0, 0.0, 0.75); glVertex3f(-1,  1, -1);
	glColor4f(1.0, 1.0, 0.0, 0.75); glVertex3f(-1, -1, -1);

	glNormal3f(1, 0, 0);
	glColor4f(0.0, 0.0, 1.0, 0.75); glVertex3f( 1, -1,  1);
	glColor4f(1.0, 1.0, 1.0, 0.75); glVertex3f( 1, -1, -1);
	glColor4f(1.0, 0.0, 1.0, 0.75); glVertex3f( 1,  1, -1);
	glColor4f(0.0, 1.0, 1.0, 0.75); glVertex3f( 1,  1,  1);
	glEnd();
}


static void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glPushMatrix();
	{
		const mn::Vector &eye = camera.getEye();
		const mn::Vector center = camera.getCenter();
		gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z,
		          0, 1, 0);
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glColor3f(0.0, 0.5, 0.0);
	glVertex3f(-100, -2, -100);
	glVertex3f( 100, -2, -100);
	glVertex3f( 100, -2,  100);
	glVertex3f(-100, -2,  100);
	glEnd();

	glPushMatrix();
	glRotatef(tick * 1.0, 1.0f, 1.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); glCallList(displayLists.cube);
	glCullFace(GL_BACK);  glCallList(displayLists.cube);
	glDisable(GL_CULL_FACE);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glTranslatef(camera.getCenterX(), camera.getCenterY(), camera.getCenterZ());

	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.2, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.2, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.2);
	glEnd();


	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	wndWidth = glutGet(GLUT_SCREEN_WIDTH);
	wndHeight = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(wndWidth, wndHeight);
	glutCreateWindow("mina86");
	glutSetCursor(GLUT_CURSOR_NONE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	displayLists.cube = glGenLists(1);
	glNewList(displayLists.cube, GL_COMPILE);
	drawCube();
	glEndList();

	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutPassiveMotionFunc(handleMotion);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, handleTick, 25);

	glutWarpPointer(wndWidth >> 1, wndHeight >> 1);
	glutMainLoop();
	return 0;
}
