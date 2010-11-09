#include <stdio.h>

#include <QtGui>
#include <QObject>

#include "../gl/glwidget.hpp"
#include "../ui/glpane.hpp"
#include "../ui/playercontrolwidget.hpp"
#include "../ui/init.hpp"

#include "scene.hpp"
#include "mainwindow.hpp"

namespace mn {

namespace ui {

QWidget *createMainWindow(gl::Configuration config) {
	QMainWindow *mainWindow = new MainWindow(config);

	QDockWidget *dockPlayer =
			new QDockWidget(QObject::tr("Player controls"), mainWindow);
	dockPlayer->setAllowedAreas(Qt::TopDockWidgetArea |
	                            Qt::BottomDockWidgetArea);

	PlayerControlWidget *pcw = new PlayerControlWidget(dockPlayer);
	dockPlayer->setWidget(pcw);

	ui::GLPane *pane = new ui::GLPane(config);

	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, dockPlayer);
	mainWindow->setCentralWidget(pane);

	QObject::connect(pcw, SIGNAL(newFrameNeeded(uint, float)),
	                 pane->gl, SLOT(updateState(uint, float)));
	QObject::connect(pcw, SIGNAL(newFrameNeeded(uint, float)),
	                 pane->gl, SLOT(updateGL()));
	/* XXX This should go away... */
	QObject::connect(pane->gl, SIGNAL(needRepaint()),
	                 pane->gl, SLOT(updateGL()));

	return mainWindow;
}

}

}
