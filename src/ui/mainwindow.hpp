/*
 * src/ui/mainwindow.hpp
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
#ifndef H_MAINWINDOW_HPP
#define H_MAINWINDOW_HPP

#include <QMainWindow>
#include <QMenu>

#include "../gl/glconfig.hpp"
#include "../gl/glwidget.hpp"

#include "ui_mainwindow.h"
#include "glpane.hpp"

#include "graph/solver/abstractsolver.hpp"

namespace ui {

struct MainWindow : public QMainWindow {
	MainWindow(gl::Config theConfig, QWidget *parent = 0);

	gl::Config config;
	GLPane * pane;

	bool isSolverLoaded() const { return solver != NULL; }
	bool isSolverPlayerLoaded() const { return solverPlayerWidget != NULL; }

public slots:
	void openSettingsDialog();
	void load();
	void save();
	void openSolverSettingsDialog();
	void generateGraph();

	void loadSolver();
	void unloadSolver(bool askForConfirmation = true);
	void createSolverPlayerWidget();
	void destroySolverPlayerWidget();

protected:
	graph::solver::AbstractSolver *solver;
	QWidget *solverPlayerWidget;

	void changeEvent(QEvent *e);
	void prepare();

	bool isFileLoaded;

	bool chooseSolverByDialog();

protected slots:
	void onWidgetSceneChanged();
	void loadScene(gl::AbstractScene::ptr scene);

private:
	MainWindow();
	MainWindow(const MainWindow &);

	Ui::MainWindow ui;
	void initActions();

	QMenu *fileMenu;
	QMenu *solverMenu;
	QAction *saveAction;
	QAction *loadAction;
	QAction *closeAction;
	QAction *quitAction;
	QAction *settingsAction;
	QAction *generateAction;
	QAction *solveAction;
	QAction *solverSettingsAction;
	QAction *solverPlayerAction;

	Q_OBJECT
};

}

#endif
