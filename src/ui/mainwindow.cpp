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

#include "dialogs/autosettingsdialog.hpp"
#include "playercontrolwidget.hpp"
#include "../graph/solver/evo/evolutionarysolver.hpp"
#include "../graph/solver/force/forcesolver.hpp"
#include "../graph/generator/euclideannetworkgenerator.hpp"
#include "../graph/generator/ergenerator.hpp"
#include "../graph/generator/treegenerator.hpp"
#include "../graph/randomise-graph.hpp"
#include "../gl/abstract-scene.hpp"

#include <fstream>

#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>

namespace ui {

MainWindow::MainWindow(gl::Config theConfig, QWidget *parent)
	: QMainWindow(parent), config(theConfig),
	  pane(NULL), solver(NULL), solverPlayerWidget(NULL) {

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
	AutoSettingsDialog *settingsDialog = new AutoSettingsDialog(&config, this);
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

	loadScene(scene);
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
	solverMenu = ui.menubar->addMenu(tr("Sol&ver", "menu"));

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

	closeAction = new QAction(this);
	closeAction->setText(tr("Close", "action to close file/window"));
	closeAction->setToolTip("Close this window");
	closeAction->setShortcut(QKeySequence::Close);
	connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

	settingsAction = new QAction(tr("Settings", "menu"), this);
	settingsAction->setToolTip(tr("Change settings of simulation"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

	generateAction = new QAction(this);
	generateAction->setText(tr("&Generate", "action to generate scene"));
	generateAction->setToolTip(tr("Generate scene"));
	connect(generateAction, SIGNAL(triggered()), this, SLOT(generateGraph()));

	solveAction = new QAction(this);
	solveAction->setText(tr("Load solver", "action to start solving a scene"));
	solveAction->setToolTip(tr("Choose a solver and start solving"));
	solveAction->setEnabled(false);
	connect(solveAction, SIGNAL(triggered()), this, SLOT(loadSolver()));

	solverSettingsAction = new QAction(tr("Solver settings", "menu"), this);
	solverSettingsAction->setToolTip(tr("Change settings of chosen solver"));
	solverSettingsAction->setEnabled(false);
	connect(solverSettingsAction, SIGNAL(triggered()), this, SLOT(openSolverSettingsDialog()));

	solverPlayerAction = new QAction(tr("Solver player"), this);
	solverPlayerAction->setToolTip(tr("Open solver player widget"));
	solverPlayerAction->setEnabled(false);
	solverPlayerAction->setCheckable(true);
	solverPlayerAction->setChecked(true);

	randomiseAction = new QAction(tr("Randomise"), this);
	randomiseAction->setEnabled(false);
	connect(randomiseAction, SIGNAL(triggered()), this, SLOT(randomiseGraph()));

	fileMenu->addAction(settingsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(loadAction);
	fileMenu->addAction(saveAction);
	fileMenu->addAction(closeAction);
	fileMenu->addSeparator();
	fileMenu->addAction(quitAction);

	solverMenu->addAction(solveAction);
	solverMenu->addAction(solverPlayerAction);
	solverMenu->addAction(solverSettingsAction);

	ui.menubar->addAction(settingsAction);

	ui.toolBar->addAction(loadAction);
	ui.toolBar->addAction(saveAction);
	ui.toolBar->addAction(generateAction);
	ui.toolBar->addAction(solveAction);
	ui.toolBar->addAction(randomiseAction);
}

void MainWindow::onWidgetSceneChanged()
{
	bool scene = pane->gl->getScene();
	saveAction->setEnabled(scene);
	randomiseAction->setEnabled(scene);
	solveAction->setEnabled(scene);
}

void MainWindow::loadScene(gl::AbstractScene::ptr scene)
{
	if (pane) {
		MainWindow *anotherWindow = new MainWindow(config);
		anotherWindow->loadScene(scene);
		anotherWindow->show();
		return;
	}

	pane = new GLPane(config);

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

	onWidgetSceneChanged();
}

void MainWindow::openSolverSettingsDialog() {
	if (solver && solver->getConfigData()) {
		AutoSettingsDialog *settingsDialog = new AutoSettingsDialog(solver->getConfigData(), this);
		settingsDialog->show();
	}
}

void MainWindow::generateGraph() {
	QString genNameEcnet = tr("Euclidean network");
	QString genNameER = tr("Erdos-Renyi");
	QString genNameTree = tr("Tree");
	QStringList generators;
	generators << genNameEcnet;
	generators << genNameER;
	generators << genNameTree;

	bool ok = false;
	QString genNameChosen;

	genNameChosen = QInputDialog::getItem(this, tr("Choose generator"), tr("Generator"),
										  generators, 0, false, &ok);

	if (!ok)
		return;

	graph::generator::AbstractGenerator *gen = NULL;
	if (genNameChosen == genNameEcnet) {
		gen = new graph::generator::EuclideanNetworkGenerator();
	} else if (genNameChosen == genNameER) {
		gen = new graph::generator::ERGenerator();
	} else if (genNameChosen == genNameTree) {
		gen = new graph::generator::TreeGenerator();
	}

	AutoSettingsDialog *genParamsDialog =
		new AutoSettingsDialog(gen->getConfigData(), this);
	genParamsDialog->setModal(true);
	genParamsDialog->exec();
	delete genParamsDialog;

	graph::generator::AbstractGenerator::graph_ptr graph = gen->generate();
	delete gen;

	gl::AbstractScene::ptr ptr(new graph::Scene(*graph));

	loadScene(ptr);
}

void MainWindow::loadSolver() {
	if (isSolverLoaded()) {
		QMessageBox::StandardButton sb = QMessageBox::question(
				this,
				tr("Solver already loaded"),
				tr("There's already a solver loaded. Do you want to unload it first?"),
				QMessageBox::Yes | QMessageBox::Cancel,
				QMessageBox::Cancel);
		switch (sb) {
		case QMessageBox::Yes:
			unloadSolver();
			break;
		default:
			return;
		}
	}

	if (chooseSolverByDialog()) {
		connect(solver, SIGNAL(graphChanged()), pane->gl, SLOT(updateGL()));
		solverSettingsAction->setEnabled(true);
		createSolverPlayerWidget();
	}
}

void MainWindow::unloadSolver(bool askForConfirmation) {
	if (askForConfirmation) {
		QMessageBox::StandardButton sb = QMessageBox::question(
				this,
				tr("Unload solver?"),
				tr("Unloading solver is irreversible. You may be able to start a new solver where this stopped, but it won't be the same as simply continuing with current solver.  Are you sure you want to unload the solver?"),
				QMessageBox::Yes | QMessageBox::Cancel,
				QMessageBox::Cancel);
		switch (sb) {
		case QMessageBox::Yes:
			break;
		default:
			return;
		}
	}
	delete solver;
	solver = NULL;
	destroySolverPlayerWidget();
	solverSettingsAction->setEnabled(false);
}

bool MainWindow::chooseSolverByDialog() {
	if (!pane || !pane->gl->getScene()) {
		QMessageBox::warning(
				this,
				tr("Unable to choose solver"),
				tr("Unable to choose solver -- there's no scene loaded. Load or generate one."),
				QMessageBox::Cancel,
				QMessageBox::Cancel);
		return false;
	}

	QString solverNameEvo = tr("Evolutionary solver");
	QString solverNameForce = tr("Physical forces solver");
	QStringList solvers;
	solvers << solverNameEvo;
	solvers << solverNameForce;

	bool ok = false;
	QString solverNameChosen;

	solverNameChosen = QInputDialog::getItem(
			this, tr("Choose solver"), tr("Solver"),
			solvers, 0, false, &ok);
	if (!ok)
		return false;

	if (solverNameChosen == solverNameEvo) {
		solver = new graph::solver::EvolutionarySolver(
			dynamic_cast<graph::Scene &>(*pane->gl->getScene())
		);
		return true;
	} else if (solverNameChosen == solverNameForce) {
		solver = new graph::solver::ForceSolver(
			dynamic_cast<graph::Scene &>(*pane->gl->getScene())
		);
		return true;
	}
	return false;
}

void MainWindow::createSolverPlayerWidget() {
	if (!isSolverLoaded()) {
		QMessageBox::warning(
				this,
				tr("Solver not loaded"),
				tr("Unable to create solver player widget because no solver is loaded."),
				QMessageBox::Ok);
		return;
	}

	if (isSolverPlayerLoaded()) {
		QMessageBox::StandardButton sb = QMessageBox::question(
				this,
				tr("Solver player already loaded"),
				tr("There's already a solver player loaded. Do you want to unload it first?"),
				QMessageBox::Yes | QMessageBox::Cancel,
				QMessageBox::Cancel);
		switch (sb) {
		case QMessageBox::Yes:
			destroySolverPlayerWidget();
			break;
		default:
			return;
		}
	}

	QDockWidget *dockWidget = new QDockWidget(tr("Player controls", "widget name"), this);
	dockWidget->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
	dockWidget->setWidget(solver->createPlayerWidget(dockWidget));
	dockWidget->setVisible(solverPlayerAction->isChecked());
	addDockWidget(Qt::BottomDockWidgetArea, dockWidget);
	connect(dockWidget, SIGNAL(visibilityChanged(bool)), solverPlayerAction, SLOT(setChecked(bool)));
	connect(solverPlayerAction, SIGNAL(toggled(bool)), dockWidget, SLOT(setVisible(bool)));
	solverPlayerWidget  = dockWidget;
	solverPlayerAction->setEnabled(true);
}

void MainWindow::destroySolverPlayerWidget() {
	if (!isSolverPlayerLoaded()) {
		QMessageBox::warning(
				this,
				tr("Solver player not loaded"),
				tr("Unable to close solver player widget because no solver is loaded."),
				QMessageBox::Ok);
		return;
	}

	delete solverPlayerWidget;
	solverPlayerWidget = NULL;
	solverPlayerAction->setEnabled(false);
}

void MainWindow::randomiseGraph() {
	graph::GraphRandomiser *randomiser = new graph::GraphRandomiser();

	AutoSettingsDialog *rndParamsDialog =
		new AutoSettingsDialog(&(randomiser->getConfigData()), this);
	rndParamsDialog->setModal(true);
	rndParamsDialog->exec();
	delete rndParamsDialog;

	randomiser->randomise(*dynamic_cast<graph::Scene *>(pane->gl->getScene()));
	delete randomiser;

	pane->gl->updateGL();
}

}
