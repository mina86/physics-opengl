#include <stdio.h>

#include <unistd.h>
#include <getopt.h>

#include <QtGui>
#include <QObject>

#include "../common/glwidget.hpp"
#include "../common/glpane.hpp"
#include "../common/texture.hpp"
#include "../common/playercontrolwidget.hpp"

#include "data-loader.hpp"
#include "scene.hpp"
#include "mainwindow.hpp"

namespace mn {

namespace ui {

int initialize(int argc, char **argv, QWidget *&window) {
	gl::Configuration config;

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
		case 'x': config->showTextures = false; break;
		case 'c': config->lowQuality = true; break;
		case 'n': config->showText = false; break;
		// case 'j': mn::physics::headlight = false; break;
		// case 'm': mn::physics::displayStars = false; break;
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
		/* FALL THROUGH */
	case  0: mn::gl::Texture::filename_suffix = ".lq.sgi"; break;
	case  1: mn::gl::Texture::filename_suffix = ".mq.sgi"; break;
	case  2: mn::gl::Texture::filename_suffix = ".hq.sgi"; break;
	}

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
	physics::Object *objects = physics::loadData(data);
	if (!objects) {
		return 1;
	}

	gl::AbstractScene::ptr scene(new physics::Scene(objects));

	QMainWindow *mainWindow = new MainWindow(config);

	QDockWidget *dockPlayer =
			new QDockWidget(QObject::tr("Player controls"), mainWindow);
	dockPlayer->setAllowedAreas(Qt::TopDockWidgetArea |
	                            Qt::BottomDockWidgetArea);

	PlayerControlWidget *pcw = new PlayerControlWidget(dockPlayer);
	dockPlayer->setWidget(pcw);

	ui::GLPane *pane = new ui::GLPane(config);
	pane->gl->setScene(scene);

	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dockPlayer);
	mainWindow->setCentralWidget(pane);

	QObject::connect(pcw, SIGNAL(nextFramePlayed()),
	                 pane->gl, SLOT(updateGL()));
	/* XXX This should go away... */
	QObject::connect(pane->gl, SIGNAL(needRepaint()),
	                 pane->gl, SLOT(updateGL()));

	window = mainWindow;
	return 0;
}

}

}
