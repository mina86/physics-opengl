#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#include <QApplication>
#include <QDesktopWidget>

#include "../lib/sintable.hpp"
#include "../lib/rand.hpp"

#include "../gl/glconfig.hpp"
#include "../gl/text3d.hpp"
#include "../gl/texture.hpp"

#include "mainwindow.hpp"

int main(int argc, char **argv) {
	lib::initSinTable();
	lib::initRand();

	QApplication app(argc, argv);
	int quality = 3, opt;
	QWidget *window;

	gl::Config config;

	static const struct option longopts[] = {
		{ "very-low",    0, 0, '0' },
		{ "low",         0, 0, '1' },
		{ "medium",      0, 0, '2' },
		{ "high",        0, 0, '3' },
		{ "no-textures", 0, 0, 't' },
		{ "low-detail",  0, 0, 'l' },
		{ "no-names",    0, 0, 'n' },
		{ "no-stars",    0, 0, 's' },
		{ "help",        0, 0, 'h' },
		{ 0, 0, 0, 0 }
	};

	while ((opt = getopt_long(argc, argv, "0123tlnsh", longopts, 0))!=-1){
		switch (opt) {
		case '0':
		case '1':
		case '2':
		case '3': quality = opt - '1'; break;
		case 'x': config->showTextures = false; break;
		case 'c': config->lowQuality = true; break;
		case 'n': config->showText = false; break;
		case 'm': config->showStars = false; break;
		case '?':
			puts("usage: ./solar [ <options> ] [ <data-file> ]\n"
				 "<options>:\n"
				 " -0 --very-low       like -1 with NEAREST textures filter\n"
				 " -1 --low            use low quality textures\n"
				 " -2 --medium         use medium quality textures\n"
				 " -3 --high           use high quality textures\n"
				 "  folowing can be toggled during runtime:\n"
				 " -t --no-textures    do not use display textures\n"
				 " -l --low-detail     use fewer vertices\n"
				 " -n --no-names       do not display names\n"
				 " -s --no-stars       do not display stars");
			return 0;
		default:
			return 1;
		}
	}

	switch (quality) {
	case -1:
		gl::Texture::useNearest = true;
		quality = 0;
		/* FALL THROUGH */
	case  0: gl::Texture::filename_suffix = ".lq.sgi"; break;
	case  1: gl::Texture::filename_suffix = ".mq.sgi"; break;
	case  2: gl::Texture::filename_suffix = ".hq.sgi"; break;
	}

	try {
		t3d::init("data/charset");
	}
	catch (const t3d::load_exception &e) {
		fprintf(stderr, "Error initialising 3D text: %s\n", e.what());
		return 1;
	}

	window = new ui::MainWindow(config);
	window->show();

	int ret = app.exec();

	delete window;

	return ret;
}
