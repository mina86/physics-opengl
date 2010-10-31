#include <stdio.h>

#include <QApplication>
#include <QDesktopWidget>

#include "application.hpp"
#include "text3d.hpp"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	QWidget *window;
	int ret;

	try {
		t3d::init("data/charset");
	}
	catch (const t3d::load_exception &e) {
		fprintf(stderr, "Error initialising 3D text: %s\n", e.what());
		return 1;
	}

	ret = mn::ui::initialize(argc, argv, window);
	if (ret) {
		return ret;
	}

	window->resize(window->sizeHint());

	if (3 * QApplication::desktop()->width() >= 4 * window->width() &&
	    3 * QApplication::desktop()->height() >= 4 * window->height()) {
		window->show();
	} else {
		window->showMaximized();
	}

	return app.exec();
}
