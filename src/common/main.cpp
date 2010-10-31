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

	window->show();

	return app.exec();
}
