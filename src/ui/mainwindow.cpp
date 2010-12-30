/*
 * src/ui/mainwindow.cpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "mainwindow.hpp"

#include "../gl/abstract-scene.hpp"
#include "../gl/glwidget.hpp"
#include "../graph/scene.hpp"

#include "dialogs/settingsdialog.hpp"
#include "playercontrolwidget.hpp"
#include "../graph/solver/dummysolver.hpp"

#include <fstream>

#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>

namespace ui {

MainWindow::MainWindow(gl::Config theConfig, QWidget *parent)
	: QMainWindow(parent), config(theConfig),
	  pane(NULL), isFileLoaded(false) {

	ui.setupUi(this);

	initActions();
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui.retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::openSettingsDialog()
{
	SettingsDialog *settingsDialog = new SettingsDialog(this, config);
	settingsDialog->show();
}

static bool fileDialog(QWidget *parent, bool save, QString &filename) {
	QStringList filters;
	filters << QWidget::tr("Scene files (*.%1)").arg(gl::AbstractScene::extension)
			<< QWidget::tr("Any file (*)");

	QFileDialog dialog(parent);
	dialog.setAcceptMode(save ? QFileDialog::AcceptSave
	                          : QFileDialog::AcceptOpen);
	if (save) {
		dialog.setConfirmOverwrite(true);
	}
	dialog.setDefaultSuffix(gl::AbstractScene::extension);
	dialog.setFileMode(save ? QFileDialog::AnyFile
	                        : QFileDialog::ExistingFile);
	dialog.setNameFilters(filters);
	dialog.setWindowTitle(save ? QWidget::tr("Save Scene")
	                           : QWidget::tr("Load Scene"));

	if (dialog.exec() == QDialog::Rejected) {
		return false;
	}

	filename = dialog.selectedFiles().front();
	return true;
}

void MainWindow::load()
{
	QString fileName;
	if (!fileDialog(this, false, fileName)) {
		return;
	}

	std::ifstream fileStream(fileName.toStdString().c_str());
	if (!fileStream) {
		QMessageBox::critical(this, tr("Error reading file"),
		                      tr("Unable to open file %1").arg(fileName));
		return;
	}

	gl::AbstractScene::ptr scene;
	try {
		scene = gl::AbstractScene::load(fileStream);
	} catch (const lib::Lexer::error &e) {
		QMessageBox::critical(this, tr("Error reading file"),
		                      tr("Parser returned error:\n%1:%2:%3:\n\t%4").arg(fileName).arg(e.location.begin.line).arg(e.location.begin.column).arg(e.what()));
		return;
	}

	if (!isFileLoaded) {
		loadScene(scene);
	} else {
		MainWindow *anotherWindow = new MainWindow(config);
		anotherWindow->loadScene(scene);
		anotherWindow->show();
	}
}

void MainWindow::save()
{
	gl::AbstractScene *sc = pane->gl->getScene();
	if (!sc) {
		QMessageBox::warning(this, tr("No scene to save!"), tr("There's no scene loaded, so there's nothing to save."), QMessageBox::Cancel);
		return;
	}

	QString fileName;
	if (!fileDialog(this, true, fileName)) {
		return;
	}

	std::ofstream fileStream(fileName.toStdString().c_str());
	if (!fileStream) {
		QMessageBox::critical(this, tr("Error reading file"),
		                      tr("Unable to open file %1").arg(fileName));
		return;
	}

	sc->save(fileStream);
}

void MainWindow::initActions()
{
	fileMenu = ui.menubar->addMenu(tr("&File", "menu"));

	quitAction = new QAction(tr("&Quit", "quit app"), this);
	quitAction->setShortcut(Qt::CTRL + Qt::Key_Q);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	saveAction = new QAction(this);
	saveAction->setText(tr("&Save", "action to save file"));
	saveAction->setToolTip("Save scene file");
	saveAction->setShortcut(QKeySequence::Save);
	saveAction->setEnabled(false);
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	loadAction = new QAction(this);
	loadAction->setText(tr("&Load", "action to load file"));
	loadAction->setToolTip("Open scene file");
	loadAction->setShortcut(QKeySequence::Open);
	connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));

	settingsAction = new QAction(tr("Settings", "menu"), this);
	settingsAction->setToolTip(tr("Change settings of simulation"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

	fileMenu->addAction(settingsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(loadAction);
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	ui.toolBar->addAction(loadAction);
	ui.toolBar->addAction(saveAction);
	ui.toolBar->addAction(settingsAction);
}

void MainWindow::onWidgetSceneChanged()
{
	saveAction->setEnabled(pane->gl->getScene());
}

void MainWindow::loadScene(gl::AbstractScene::ptr scene)
{
	pane = new GLPane(config);

//	QDockWidget *dockPlayer = new QDockWidget(tr("Player controls"), this);
//	dockPlayer->setAllowedAreas(Qt::TopDockWidgetArea |
//	                            Qt::BottomDockWidgetArea);

//	PlayerControlWidget *pcw = new PlayerControlWidget(dockPlayer);
//	dockPlayer->setWidget(pcw);

//	addDockWidget(Qt::BottomDockWidgetArea, dockPlayer);

	connect(pane->gl, SIGNAL(sceneChanged()),
	        this, SLOT(onWidgetSceneChanged()));
//	connect(pcw, SIGNAL(newFrameNeeded(uint, float)),
//	        pane->gl, SLOT(updateState(uint, float)));
//	connect(pcw, SIGNAL(newFrameNeeded(uint, float)),
//	        pane->gl, SLOT(updateGL()));
//	/* XXX This should go away... */
	connect(pane->gl, SIGNAL(needRepaint()),
	        pane->gl, SLOT(updateGL()));

	setCentralWidget(pane);
	pane->gl->setScene(scene);
	isFileLoaded = true;

	onWidgetSceneChanged();

	graph::DummySolver *solver = new graph::DummySolver(this);
	solver->scene = pane->gl->getScene<graph::Scene>();
	solver->createPlayerWidget(this)->show();
	connect(solver, SIGNAL(oneStepMade()), pane->gl, SLOT(updateGL()));
}

}
