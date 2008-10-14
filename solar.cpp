#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "camera.hpp"
#include "planet.hpp"
#include "sintable.hpp"
#include "text3d.hpp"

static mn::Sphere *sun;


static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (!down) return;
	switch (key) {
	case 27: /* Escape */
		glutLeaveGameMode();
		exit(0);
	case ' ':
		mn::Camera::tickRedisplays = mn::Camera::countTicks = !mn::Camera::countTicks;
		break;
	}
}

static void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 10000.0);
}


struct pushMatrix {
	pushMatrix() { glPushMatrix(); }
	~pushMatrix() { glPopMatrix(); }
private:
	pushMatrix(const pushMatrix &pm) { (void)pm; }
};


static void drawScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mn::Camera &camera = *mn::Camera::getDefaultCamera();

	{
		pushMatrix pm;
		camera.doLookAt();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		GLfloat lightColor0[] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat lightPos0[] = {0, 0, 0, 1.0f};
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

		sun->draw(mn::Camera::getTicks());

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
	}

	{
		pushMatrix pm;

		glTranslatef(-0.8, 0.8, -2);
		glScalef(0.03, 0.03, 0.03);
		char buffer[1024];
		const mn::Vector &eye = camera.getEye();
		sprintf(buffer, "position = (%3.3f, %3.3f, %3.3f)\nrotation = (%3.3f, %3.3f, %3.3f)", eye.x, eye.y, eye.z, camera.getRotX() * (180/M_PI), camera.getRotY() * (180/M_PI), 0.0);
		glColor3f(1, 1, 1);
		t3d::draw2D(std::string(buffer), -1, -1);
	}

	glTranslatef(-1, -0.8, 0);
	camera.doLookAt();
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
	mn::initSinTable();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutGameModeString(":32");
	glutEnterGameMode();
	glutIgnoreKeyRepeat(1);

	try {
		t3d::init();
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
	sun = new mn::Sphere(0, /*109*/ 20 * sizeFactor, 0, mn::Color(1, 1, 0),
	                     "Sun");
#define P(distance, radius, period, r, g, b, name) \
	sun->pushSatelite(*new mn::Sphere(distance * distanceFactor, \
	                                  radius * sizeFactor, \
	                                  omegaFactor/period, \
	                                  mn::Color(r, g, b), name));
	P(88.0, 0.14, 12050, 1.0, 1.0, 1.0, "90377 Sedna");
	P(67.7, 0.19, 557.0, 1.0, 1.0, 1.0, "136199 Eris");
	P(39.5, 0.19, 248.1, 1.0, 1.0, 1.0, "134340 Pluto");
	P(30.1, 3.88, 164.9, 1.0, 1.0, 1.0, "Neptune");
	P(19.2, 4.00, 84.07, 1.0, 1.0, 1.0, "Uranus");
	P(9.58, 9.45, 29.45, 1.0, 1.0, 1.0, "Saturn");
	P(5.20, 11.2, 11.87, 1.0, 1.0, 1.0, "Jupiter");
	P(3.14, 0.03, 5.557, 1.0, 1.0, 1.0, "10 Hygiea");
	P(2.77, 0.07, 4.600, 1.0, 1.0, 1.0, "1 Ceres");
	P(2.36, 0.04, 3.629, 1.0, 1.0, 1.0, "4 Vesta");
	P(1.50, 0.53, 1.881, 1.0, 0.0, 0.0, "Mars");
	P(1.00, 1.00, 1.000, 0.0, 1.0, 1.0, "Earth");
	P(0.73, 0.95, 0.615, 1.0, 1.0, 1.0, "Venus");
	P(0.38, 0.38, 0.241, 1.0, 1.0, 1.0, "Mercury");
#undef P

	mn::Camera camera;
	mn::Camera::setDefaultCamera(&camera);
	mn::Camera::setSize(glutGet(GLUT_SCREEN_WIDTH),
	                    glutGet(GLUT_SCREEN_HEIGHT));

	mn::Camera::keyMovementFactor   *=  3;
	mn::Camera::keyRunFactor        *=  3;
	mn::Camera::mouseMovementFactor *=  3;
	mn::Camera::mouseRunFactor      *=  3;

	mn::Camera::registerHandlers();
	mn::Camera::setResizeFunc(handleResize);
	mn::Camera::setKeyboardFunc(handleKeyboard);
	glutDisplayFunc(drawScene);

	glutMainLoop();
	return 0;
}
