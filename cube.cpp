#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "camera.hpp"


static struct { GLuint cube; } displayLists;



static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (down && key == 27) {
		exit(0);
	}
}

static void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
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
	mn::Camera::defaultLookAt();

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
	glRotatef(mn::Camera::getTicks(), 1.0f, 1.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); glCallList(displayLists.cube);
	glCullFace(GL_BACK);  glCallList(displayLists.cube);
	glDisable(GL_CULL_FACE);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);
	glTranslatef(mn::Camera::getDefaultCamera()->getCenterX(),
	             mn::Camera::getDefaultCamera()->getCenterY(),
	             mn::Camera::getDefaultCamera()->getCenterZ());

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

	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(w, h);
	glutCreateWindow("Spinning Cube");
	glutSetCursor(GLUT_CURSOR_NONE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	displayLists.cube = glGenLists(1);
	glNewList(displayLists.cube, GL_COMPILE);
	drawCube();
	glEndList();

	mn::Camera camera;
	mn::Camera::setDefaultCamera(&camera);
	mn::Camera::setSize(w, h);

	mn::Camera::registerHandlers();
	mn::Camera::setResizeFunc(handleResize);
	mn::Camera::setKeyboardFunc(handleKeyboard);
	glutDisplayFunc(drawScene);

	glutMainLoop();
	return 0;
}
