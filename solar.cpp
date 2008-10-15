#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "camera.hpp"
#include "sphere.hpp"
#include "sintable.hpp"
#include "text3d.hpp"
#include "quadric.hpp"


static mn::solar::Sphere *sun;


static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (!down) return;
	switch (key) {
	case 27: /* Escape */
		exit(0);

	case 'v': case 'V':
		mn::gl::Quadric::quadric()->setNextDrawStyle();
		break;

	case 'b': case 'B':
		mn::solar::Sphere::drawOrbits = !mn::solar::Sphere::drawOrbits;
		break;

	case 'n': case 'N':
		mn::solar::Sphere::drawNames = !mn::solar::Sphere::drawNames;
		break;

	case ' ':
		mn::gl::Camera::tickRedisplays = mn::gl::Camera::countTicks = !mn::gl::Camera::countTicks;
		break;

	default:
		return;
	}

	glutPostRedisplay();
}

static void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 0.01, 10000.0);
}


struct pushMatrix {
	pushMatrix() { glPushMatrix(); }
	~pushMatrix() { glPopMatrix(); }
private:
	pushMatrix(const pushMatrix &pm) { (void)pm; }
};


static unsigned fps_counter = 0;
static float fps = 0;
static void zeroFPS(int param) {
	fps = fps * 0.25 + fps_counter * 0.75;
	fps_counter = 0;
	glutPostRedisplay();
	glutTimerFunc(param, zeroFPS, param);
}

static void drawScene() {
	++fps_counter;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const mn::gl::Camera &camera = *mn::gl::Camera::getDefaultCamera();
	const mn::gl::Vector &eye = camera.getEye();

	{
		pushMatrix pm;

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);

		camera.doLookAt();

		static const GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f};
		static const GLfloat lightPos0[] = {0, 0, 0, 1.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

		glDisable(GL_COLOR_MATERIAL);
		sun->draw(mn::gl::Camera::getTicks(), mn::gl::Vector(0, 0, 0));
		glEnable(GL_COLOR_MATERIAL);

		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}

	{
		pushMatrix pm;

		glTranslatef(0, 0, -2);

		glColor3f(0, 0.7, 0);
		glBegin(GL_LINES);
		glVertex3f(-0.01,  0.01, 0); glVertex3f(-0.02,  0.02, 0);
		glVertex3f(-0.01, -0.01, 0); glVertex3f(-0.02, -0.02, 0);
		glVertex3f( 0.01, -0.01, 0); glVertex3f( 0.02, -0.02, 0);
		glVertex3f( 0.01,  0.01, 0); glVertex3f( 0.02,  0.02, 0);
		glEnd();

		glTranslatef(-0.8, 0.8, 0);
		glScalef(0.03, 0.03, 0.03);
		char buffer[1024];
		sprintf(buffer, "position = (%3.3f, %3.3f, %3.3f)\nrotation = (%3.3f, %3.3f, %3.3f)\nfps = %3.1f", eye.x, eye.y, eye.z, camera.getRotX() * (180/M_PI), camera.getRotY() * (180/M_PI), 0.0, fps);
		glColor3f(1, 1, 1);
		t3d::draw2D(std::string(buffer), -1, -1);
	}

	glTranslatef(-1, -0.8, 0);
	camera.doLookAt();
	glTranslatef(mn::gl::Camera::getDefaultCamera()->getCenterX(),
	             mn::gl::Camera::getDefaultCamera()->getCenterY(),
	             mn::gl::Camera::getDefaultCamera()->getCenterZ());
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.2, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.2, 0);
	glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.2);
	glEnd();

	glutSwapBuffers();
}


int main(int argc, char** argv) {
	mn::initSinTable();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	int w = glutGet(GLUT_SCREEN_WIDTH);
	int h = glutGet(GLUT_SCREEN_HEIGHT);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Spinning Cube");
	glutSetCursor(GLUT_CURSOR_NONE);

	try {
		t3d::init("data/charset");
	}
	catch (const t3d::load_exception &e) {
		fprintf(stderr, "Error initialising 3D text: %s\n", e.what());
		return 1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	const float distanceFactor = 149597870691e-10;  /* AU */
	const float sizeFactor = 6371000e-8; /* Earth's mean radius */
	const float omegaFactor = 0.1;

	sun = new mn::solar::Sphere(0, /*109*/ 20 * sizeFactor, 0,
	                            mn::gl::color(1, 1, 0), "Sun");
	mn::solar::Sphere *top = sun;
#define P(distance, radius, period, r, g, b, name) \
	top->pushSatelite(*new mn::solar::Sphere(distance * distanceFactor,	\
	                                         radius * sizeFactor, \
	                                         omegaFactor/period, \
	                                         mn::gl::color(r, g, b), name));
	P(0.38, 0.38, 0.241, 1.0, 1.0, 1.0, "Mercury");
	P(0.73, 0.95, 0.615, 1.0, 1.0, 1.0, "Venus");
	top = &P(1.00, 1.00, 1.000, 0.0, 1.0, 1.0, "Earth");
	P(0.01, 0.01, 0.074, 0.8, 0.8, 0.8, "Moon");
	top = sun;
	P(1.50, 0.53, 1.881, 1.0, 0.0, 0.0, "Mars");
	P(2.36, 0.04, 3.629, 1.0, 1.0, 1.0, "4 Vesta");
	P(2.77, 0.07, 4.600, 1.0, 1.0, 1.0, "1 Ceres");
	P(3.14, 0.03, 5.557, 1.0, 1.0, 1.0, "10 Hygiea");
	P(5.20, 11.2, 11.87, 1.0, 1.0, 1.0, "Jupiter");
	P(9.58, 9.45, 29.45, 1.0, 1.0, 1.0, "Saturn");
	P(19.2, 4.00, 84.07, 1.0, 1.0, 1.0, "Uranus");
	P(30.1, 3.88, 164.9, 1.0, 1.0, 1.0, "Neptune");
	P(39.5, 0.19, 248.1, 1.0, 1.0, 1.0, "134340 Pluto");
	P(67.7, 0.19, 557.0, 1.0, 1.0, 1.0, "136199 Eris");
	P(88.0, 0.14, 12050, 1.0, 1.0, 1.0, "90377 Sedna");
#undef P

	mn::gl::Camera camera;
	mn::gl::Camera::setDefaultCamera(&camera);
	mn::gl::Camera::setSize(w, h);

	mn::gl::Camera::keyMovementFactor   *=  3;
	mn::gl::Camera::mouseMovementFactor *=  3;

	mn::gl::Camera::registerHandlers();
	mn::gl::Camera::setResizeFunc(handleResize);
	mn::gl::Camera::setKeyboardFunc(handleKeyboard);
	glutDisplayFunc(drawScene);


	static const GLfloat lightColor1[] = {0.0f, 1.0f, 1.0f, 1.0f};
	static const GLfloat lightPos1[] = { 0, 0, 0, 0 };
	static const GLfloat lightDirection1[] = { 0, 0, 0, -1 };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection1);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 15.f);

	glutTimerFunc(1000, zeroFPS, 1000);

	glutMainLoop();
	return 0;
}
