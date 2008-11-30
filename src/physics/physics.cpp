#ifndef _GNU_SOURCE
#  define _GNU_SOURCE 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

#include <vector>
#include <limits>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

#include "../common/camera.hpp"
#include "../common/sintable.hpp"
#include "../common/text3d.hpp"
#include "../common/quadric.hpp"
#include "../common/texture.hpp"
#include "../common/mconst.h"
#include "object.hpp"
#include "data-loader.hpp"


namespace mn {

namespace physics {

static Object *objects;
static const Object *tabPosition = 0;
static bool headlight = true, displayStars = true;
static gl::Texture starsTexture(GL_LUMINANCE, GL_LUMINANCE);


static void handleKeyboard(unsigned key, bool down, int x, int y) {
	(void)x; (void)y;
	if (!down) return;
	switch (key) {
	case 27: /* Escape */
		exit(0);

	case '\t':
		tabPosition = (tabPosition ? tabPosition : objects)->getNext();
		gl::Camera::camera->moved = false;
		break;

	case 'x': case 'X':
		Object::useTextures = !Object::useTextures;
		break;

	case 'c': case 'C':
		Object::lowQuality = !Object::lowQuality;
		break;

	case 'v': case 'V':
		gl::Quadric::quadric()->setNextDrawStyle();
		break;

	case 'n': case 'N':
		Object::drawNames = !Object::drawNames;
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

	gl::Camera::nextTickRedisplays = true;
}


namespace {

struct PushMatrix {
	PushMatrix() { glPushMatrix(); }
	~PushMatrix() { glPopMatrix(); }
};

}


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

	for (unsigned i = 0; i < 180; i += stacks_deg) {
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

	mn::gl::Camera &camera = *mn::gl::Camera::camera;

	if (!camera.moved && tabPosition) {
		camera.setEye(tabPosition->getPosition());
		camera.moveZ(5);
		camera.moved = false;
	}

	const mn::gl::Vector<float> &eye = camera.getEye();

	{
		PushMatrix pm;

		camera.doRotate();
		drawStars();
		camera.doMove();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);

		objects->drawAll();

		glDisable(GL_LIGHTING);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
	}

	{
		PushMatrix pm;

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
	char buffer[2048];
	int i = sprintf(buffer,
	                "pos = (%6.2f, %6.2f, %6.2f); dist = %6.2f\n"
	                "rot = (%2.2f, %2.2f, %2.2f)\nfps = %3.1f\nspeed = %.1f",
	                eye.x, eye.y, eye.z, eye.length(),
	                camera.getRotX()*MN_180_PI, camera.getRotY()*MN_180_PI, 0.0,
	                fps,
	                mn::gl::Camera::countTicks*mn::gl::Camera::tickIncrement/10.0f);
	if (tabPosition) {
		const Object::Vector &pos = tabPosition->getPosition();
		const Object::Vector &vel = tabPosition->getVelocity();
		sprintf(buffer + i,
		        "\n\n%s\nr = (%6.2f, %6.2f, %6.2f)\nV = (%6.2f, %6.2f, %6.2f)",
		        tabPosition->getName().c_str(),
		        pos.x, pos.y, pos.z, vel.x, vel.y, vel.z);
	}
	glColor3f(1, 1, 1);
	t3d::draw2D(std::string(buffer), -1, -1);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glutSwapBuffers();


	if (gl::Camera::countTicks && gl::Camera::tickIncrement) {
		objects->ticksAll(gl::Camera::tickIncrement, 1/250.0f);
		objects->updatePointAll();
	}
}

}

}


int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	static const struct option longopts[] = {
		{ "very-low",    0, 0, '0' },
		{ "low",         0, 0, '1' },
		{ "medium",      0, 0, '2' },
		{ "high",        0, 0, '3' },
		{ "no-textures", 0, 0, 'x' },
		{ "low-detail",  0, 0, 'c' },
		{ "no-names",    0, 0, 'n' },
		{ "no-light",    0, 0, 'j' },
		{ "no-stars",    0, 0, 'm' },
		{ "help",        0, 0, '?' },
		{ 0, 0, 0, 0 }
	};
	int opt, quality = 3;
	while ((opt = getopt_long(argc, argv, "0123?xcbnjmH", longopts, 0))!=-1){
		switch (opt) {
		case '0':
		case '1':
		case '2':
		case '3': quality = opt - '1'; break;
		case 'x': mn::physics::Object::useTextures = false; break;
		case 'c': mn::physics::Object::lowQuality  = true ; break;
		case 'n': mn::physics::Object::drawNames   = false; break;
		case 'j': mn::physics::headlight = false; break;
		case 'm': mn::physics::displayStars = false; break;
		case '?':
			puts("usage: ./solar [ <options> ] [ <data-file> ]\n"
				 "<options>:\n"
				 " -0 --very-low       like -1 with NEAREST textures filter\n"
				 " -1 --low            use low quality textures\n"
				 " -2 --medium         use medium quality textures\n"
				 " -3 --high           use high quality textures\n"
				 "  folowing can be toggled during runtime:\n"
				 " -x --no-textures    do not use display textures\n"
				 " -c --low-detail     use fewer vertices\n"
				 " -n --no-names       do not display names\n"
				 " -j --no-light       turn off headlight\n"
				 " -m --no-stars       do not display stars");
			return 0;
		default:
			return 1;
		}
	}

	switch (quality) {
	case -1:
		mn::gl::Texture::useNearest = true;
		quality = 0;
	case  0: mn::gl::Texture::filename_suffix = ".lq.sgi"; break;
	case  1: mn::gl::Texture::filename_suffix = ".mq.sgi"; break;
	case  2: mn::gl::Texture::filename_suffix = ".hq.sgi"; break;
	}


	mn::initSinTable();


	{
		const char *data = optind == argc ? 0 : argv[optind], *dir;
		if (!data) {
			mn::gl::Texture::filename_prefix = "./data/";
		} else if ((dir = strrchr(data, '/'))) {
			char *path = new char[dir - data + 2];
			memcpy(path, data, dir - data + 1);
			path[dir - data + 1] = 0;
			mn::gl::Texture::filename_prefix = path;
		}

		if (!data) {
			puts("Reading data from standard input");
		}
		mn::physics::objects = mn::physics::loadData(data);
		if (!mn::physics::objects) {
			return 1;
		}
	}


	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH),
	                   glutGet(GLUT_SCREEN_HEIGHT));
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Physics");
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

		mn::physics::starsTexture.assign(size, size, stars);
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


	glutTimerFunc(1000, mn::physics::zeroFPS, 1000);


	mn::gl::Camera camera;
	mn::gl::Camera::maxDistance = std::numeric_limits<float>::infinity();
	mn::gl::Camera::camera = &camera;
	mn::gl::Camera::keyMovementFactor   *=  3;
	mn::gl::Camera::mouseMovementFactor *=  3;
	mn::gl::Camera::registerHandlers();
	mn::gl::Camera::keyboardFunc = mn::physics::handleKeyboard;
	glutDisplayFunc(mn::physics::drawScene);

	puts("\n==================== Key Bindings ====================");
	mn::gl::Camera::printHelp();
	puts("tab  follow object\n"
	     "toggle: x  textures      c  low quality   v  display mode\n"
	     "        j  head light    n  names         m  stars\n");


	glutMainLoop();
	return 0;
}

