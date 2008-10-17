#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

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
#include "data-loader.hpp"
#include "texture.hpp"


static mn::solar::Sphere *sun;
static bool headlight = true, displayStars = true;
static mn::gl::Texture starsTexture(GL_LUMINANCE, GL_LUMINANCE);


static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (!down) return;
	switch (key) {
	case 27: /* Escape */
		exit(0);

	case 'x': case 'X':
		mn::solar::Sphere::useTextures = !mn::solar::Sphere::useTextures;
		break;

	case 'c': case 'C':
		mn::solar::Sphere::lowQuality = !mn::solar::Sphere::lowQuality;
		break;

	case 'v': case 'V':
		mn::gl::Quadric::quadric()->setNextDrawStyle();
		break;

	case 'b': case 'B':
		mn::solar::Sphere::drawOrbits = !mn::solar::Sphere::drawOrbits;
		break;

	case 'n': case 'N':
		mn::solar::Sphere::drawNames = !mn::solar::Sphere::drawNames;
		break;

	case 'j': case 'J':
		if (headlight) {
			glDisable(GL_LIGHT0);
			headlight = false;
		} else {
			glEnable(GL_LIGHT0);
			headlight = true;
		}
		break;

	case 'm': case 'M':
		displayStars = !displayStars;
		break;

	case ' ':
		mn::gl::Camera::tickRedisplays = mn::gl::Camera::countTicks = !mn::gl::Camera::countTicks;
		break;

	default:
		return;
	}

	glutPostRedisplay();
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


static void drawStars() {
	if (displayStars && starsTexture) {
		static GLuint displayList = 0;
		if (displayList) {
			glCallList(displayList);
		} else {
			displayList = glGenLists(1);
			if (displayList) {
				glNewList(displayList, GL_COMPILE);
			}

			mn::gl::Quadric starsQuadric;
			gluQuadricOrientation(starsQuadric, GLU_INSIDE);
			gluQuadricTexture(starsQuadric, 1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, *starsTexture);
			glColor3f(1, 1, 1);
			gluSphere(starsQuadric, 3000, 10, 10);
			glDisable(GL_TEXTURE_2D);

			if (displayList) {
				glEndList();
				glCallList(displayList);
			}
		}
	}
}

static void drawScene() {
	++fps_counter;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	const mn::gl::Camera &camera = *mn::gl::Camera::camera;
	const mn::gl::Vector &eye = camera.getEye();

	{
		pushMatrix pm;

		camera.doLookAt();

		glEnable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);

		sun->draw(mn::gl::Camera::ticks, mn::gl::Vector(0, 0, 0));

		glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
	}

	{
		pushMatrix pm;

		glTranslatef(-1, -0.8, -3);
		camera.doRotate();

		drawStars();

	glDisable(GL_DEPTH_TEST);

		glBegin(GL_LINES);
		glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.2, 0, 0);
		glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.2, 0);
		glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.2);
		glEnd();
	}

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
	sprintf(buffer, "position = (%6.2f, %6.2f, %6.2f)\ndistance = %6.2f\nrotation = (%2.2f, %2.2f, %2.2f)\nfps = %3.1f", eye.x, eye.y, eye.z, eye.length(), camera.getRotX() * (180/M_PI), camera.getRotY() * (180/M_PI), 0.0, fps);
	glColor3f(1, 1, 1);
	t3d::draw2D(std::string(buffer), -1, -1);

	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	mn::initSinTable();

	sun = mn::solar::loadData(argc == 1 ? "data/helio.txt" : argv[1]);
	if (!sun) {
		return 1;
	}

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


	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	{
		unsigned char *stars = new unsigned char[1024 * 1024];
		memset(stars, 0, sizeof stars);
		srand(time(0));
		for (unsigned count = 0; count < 2048; ++count) {
#if RAND_MAX >= (1024 * 1024 - 1)
			const unsigned pos = rand() % (1024 * 1024);
#else
			const unsigned pos = (rand() * (RAND_MAX + 1) | rand()) % (1024 * 1024);
#endif
			stars[pos] = rand() * (256.0f / (RAND_MAX + 1.0f));
		}

		starsTexture.assign(1024, 1024, stars);
	}


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);


	glEnable(GL_LIGHT0);
	static const GLfloat lightColor[] = {0.01f, 0.4f, 0.2f, 1.0f};
	static const GLfloat lightPos[] = { 0, 0, 0, 1 };
	static const GLfloat lightDirection[] = { 0, 0, -1, 0 };

	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
	glLightf (GL_LIGHT0, GL_SPOT_CUTOFF, 2.f);

	glutTimerFunc(1000, zeroFPS, 1000);


	mn::gl::Camera camera;
	mn::gl::Camera::camera = &camera;
	mn::gl::Camera::keyMovementFactor   *=  3;
	mn::gl::Camera::mouseMovementFactor *=  3;
	mn::gl::Camera::registerHandlers();
	mn::gl::Camera::printHelp();
	puts("toggle: x  textures      c  low quality   v  display mode\n"
		 "        b  orbits        n  names         m  stars\n"
		 "        j  head light\n");
	mn::gl::Camera::keyboardFunc = handleKeyboard;
	glutDisplayFunc(drawScene);


	glutMainLoop();
	return 0;
}
