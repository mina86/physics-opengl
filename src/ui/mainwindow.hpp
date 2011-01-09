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

public slots:
	void openSettingsDialog();
	void load();
	void save();
	void openSolverSettingsDialog();

protected:
	graph::solver::AbstractSolver *solver;

	void changeEvent(QEvent *e);
	void prepare();

	bool isFileLoaded;

protected slots:
	void onWidgetSceneChanged();
	void loadScene(gl::AbstractScene::ptr scene);

private:
	MainWindow();
	MainWindow(const MainWindow &);

	Ui::MainWindow ui;
	void initActions();

	QMenu *fileMenu;
	QAction *saveAction;
	QAction *loadAction;
	QAction *quitAction;
	QAction *settingsAction;

	Q_OBJECT
};

}

#endif
