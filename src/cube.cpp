#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "common/camera.hpp"


static struct { GLuint cube; } displayLists;



static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (down && key == 27) {
		exit(0);
	}
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

	glPushMatrix();
	mn::gl::Camera::defaultLookAt();

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

	glRotatef(mn::gl::Camera::ticks, 1.0f, 1.0f, 0.0f);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); glCallList(displayLists.cube);
	glCullFace(GL_BACK);  glCallList(displayLists.cube);
	glDisable(GL_CULL_FACE);
	glPopMatrix();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	glTranslatef(-1, -0.8, -3);
	mn::gl::Camera::camera->doRotate();

	glDisable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.2, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.2, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.2);
	glEnd();
	glEnable(GL_DEPTH_TEST);

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

	mn::gl::Camera camera;
	mn::gl::Camera::camera = &camera;
	mn::gl::Camera::registerHandlers();
	mn::gl::Camera::keyboardFunc = handleKeyboard;
	glutDisplayFunc(drawScene);

	glutMainLoop();
	return 0;
}
