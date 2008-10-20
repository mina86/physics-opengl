#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

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
#include "mconst.h"


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

	default:
		return;
	}

	mn::gl::Camera::nextTickRedisplays = true;
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
	fps = (fps + (3 * fps_counter)) * 0.25f;
	fps_counter = 0;
	mn::gl::Camera::nextTickRedisplays = true;
	glutTimerFunc(param, zeroFPS, param);
}

static void drawStars() {
	if (!displayStars) return;

	if (!*starsTexture) {
		displayStars = false;
		return;
	}

	static GLuint displayList = 0;
	if (displayList) {
		glCallList(displayList);
		return;
	}

	displayList = glGenLists(1);
	if (displayList) {
		glNewList(displayList, GL_COMPILE);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, *starsTexture);
	glColor3f(1, 1, 1);

	const unsigned slices_deg = 60;
	const unsigned stacks_deg = 60;
	const float texMul = 1 / 120.0f;

	for (unsigned i = 0; i < 360; i += stacks_deg) {
		const float cos1 = mn::cos(i);
		const float sin1 = mn::sin(i);
		const float cos2 = mn::cos(i + stacks_deg);
		const float sin2 = mn::sin(i + stacks_deg);

		glBegin(GL_QUAD_STRIP);
		for (unsigned j = 0; j <= 360; j += slices_deg) {
			glTexCoord2f(j * texMul, i * texMul);
			glVertex3f(sin1 * mn::sin(j), sin1 * mn::cos(j), cos1);
			glTexCoord2f(j * texMul, (i + stacks_deg) * texMul);
			glVertex3f(sin2 * mn::sin(j), sin2 * mn::cos(j), cos2);
		}
		glEnd();
	}

	glDisable(GL_TEXTURE_2D);

	if (displayList) {
		glEndList();
		glCallList(displayList);
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

		camera.doRotate();
		drawStars();
		camera.doMove();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);

		sun->draw(mn::gl::Camera::ticks, mn::gl::Vector(0, 0, 0));

		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	{
		pushMatrix pm;

		glTranslatef(-0.4 * mn::gl::Camera::aspect(), -0.4, -1.2);
		camera.doRotate();

		glBegin(GL_LINES);
		glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(0.05, 0, 0);
		glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0.05, 0);
		glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, 0.05);
		glEnd();
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-mn::gl::Camera::aspect(), mn::gl::Camera::aspect(), -1,1, -1,1);
	glMatrixMode(GL_MODELVIEW);

	glColor3f(0, 0.5, 0);
	glBegin(GL_LINES);
	glVertex3f(-0.01,  0.01, 0); glVertex3f(-0.02,  0.02, 0);
	glVertex3f(-0.01, -0.01, 0); glVertex3f(-0.02, -0.02, 0);
	glVertex3f( 0.01, -0.01, 0); glVertex3f( 0.02, -0.02, 0);
	glVertex3f( 0.01,  0.01, 0); glVertex3f( 0.02,  0.02, 0);
	glEnd();

	glTranslatef(0.1-mn::gl::Camera::aspect(), 0.9, 0);
	glScalef(0.03, 0.03, 0.03);
	char buffer[1024];
	sprintf(buffer, "position = (%6.2f, %6.2f, %6.2f)\ndistance = %6.2f\nrotation = (%2.2f, %2.2f, %2.2f)\nfps = %3.1f\nspeed = %lu", eye.x, eye.y, eye.z, eye.length(), camera.getRotX() * MN_180_PI, camera.getRotY() * MN_180_PI, 0.0, fps, mn::gl::Camera::countTicks * mn::gl::Camera::tickIncrement);
	glColor3f(1, 1, 1);
	t3d::draw2D(std::string(buffer), -1, -1);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glutSwapBuffers();
}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	static const struct option longopts[] = {
		{ "low",         0, 0, '1' },
		{ "medium",      0, 0, '2' },
		{ "high",        0, 0, '3' },
		{ "no-textures", 0, 0, 'x' },
		{ "low-detail",  0, 0, 'c' },
		{ "no-orbits",   0, 0, 'b' },
		{ "no-names",    0, 0, 'n' },
		{ "no-light",    0, 0, 'j' },
		{ "no-stars",    0, 0, 'm' },
		{ "help",        0, 0, '?' },
		{ 0, 0, 0, 0 }
	};
	int opt, quality = 3;
	while ((opt = getopt_long(argc, argv, "123?xcbnjmH", longopts, 0)) != -1){
		switch (opt) {
		case '1':
		case '2':
		case '3': quality = opt - '1'; break;
		case 'x': mn::solar::Sphere::useTextures = false; break;
		case 'c': mn::solar::Sphere::lowQuality  = true ; break;
		case 'b': mn::solar::Sphere::drawOrbits  = false; break;
		case 'n': mn::solar::Sphere::drawNames   = false; break;
		case 'j': headlight = false; break;
		case 'm': displayStars = false; break;
		case '?':
			puts("usage: ./solar [ <options> ] [ <data-file> ]\n"
				 "<options>:\n"
				 " -1 --low            use low quality textures\n"
				 " -2 --medium         use medium quality textures\n"
				 " -3 --high           use high quality textures\n"
				 "  folowing can be toggled during runtime:\n"
				 " -x --no-textures    do not use display textures\n"
				 " -c --low-detail     use fewer vertices\n"
				 " -b --no-orbits      do not display orbits\n"
				 " -n --no-names       do not display names\n"
				 " -j --no-light       turn off headlight\n"
				 " -m --no-stars       do not display stars");
			return 0;
		default:
			return 1;
		}
	}

	switch (quality) {
	case 0: mn::gl::Texture::filename_suffix = ".lq.sgi"; break;
	case 1: mn::gl::Texture::filename_suffix = ".mq.sgi"; break;
	case 2: mn::gl::Texture::filename_suffix = ".hq.sgi"; break;
	}

	{
		const char *data = optind == argc ? "data/helio.txt" : argv[optind];
		const char *dir = strrchr(data, '/');
		if (dir) {
			char *path = new char[dir - data + 2];
			memcpy(path, data, dir - data + 1);
			path[dir - data + 1] = 0;
			mn::gl::Texture::filename_prefix = path;
		}

		sun = mn::solar::loadData(data);
		if (!sun) {
			return 1;
		}
	}


	mn::initSinTable();


	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH),
	                   glutGet(GLUT_SCREEN_HEIGHT));
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
		const unsigned size = 1 << (8 + quality), size2 = size * size;
		const unsigned mask = size2 - 1;

		unsigned char *stars = new unsigned char[size2];
		memset(stars, 0, size2);
		srand(time(0));

		unsigned count = 2 * size;
		do {
			const unsigned pos =
				((RAND_MAX<mask ? rand() * (RAND_MAX+1) : 0) + rand()) & mask;
			stars[pos] = rand() * (256.0f / (RAND_MAX + 1.0f));
		} while (--count);

		starsTexture.assign(size, size, stars);
	}


	glShadeModel(GL_SMOOTH);

	{
		glEnable(GL_LIGHT0);
		static const GLfloat lightColor[] = {0.01f, 0.4f, 0.2f, 1.0f};
		static const GLfloat lightPos[] = { 0, 0, 0, 1 };
		static const GLfloat lightDirection[] = { 0, 0, -1, 0 };

		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection);
		glLightf (GL_LIGHT0, GL_SPOT_CUTOFF, 2.f);
	}


	glutTimerFunc(1000, zeroFPS, 1000);


	mn::gl::Camera camera;
	mn::gl::Camera::camera = &camera;
	mn::gl::Camera::keyMovementFactor   *=  3;
	mn::gl::Camera::mouseMovementFactor *=  3;
	mn::gl::Camera::registerHandlers();
	mn::gl::Camera::keyboardFunc = handleKeyboard;
	glutDisplayFunc(drawScene);

	puts("\n==================== Key Bindings ====================");
	mn::gl::Camera::printHelp();
	puts("toggle: x  textures      c  low quality   v  display mode\n"
		 "        b  orbits        n  names         m  stars\n"
		 "        j  head light\n");


	glutMainLoop();
	return 0;
}
