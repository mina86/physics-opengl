#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../ui/dialogs/settingsdialog.hpp"
#include "../gl/abstract-scene.hpp"
#include "../gl/glwidget.hpp"

#include <fstream>

#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	prepare();
}

MainWindow::MainWindow(mn::gl::Configuration &config) :
		QMainWindow(0),
		config(config),
		ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	initActions();
	prepare();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::prepare()
{
	connect(ui->action_Settings, SIGNAL(activated()), this, SLOT(openSettingsDialog()));
}

void MainWindow::openSettingsDialog()
{
	SettingsDialog *settingsDialog = new SettingsDialog(this, config);
	settingsDialog->show();
}

void MainWindow::load()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load Scene"), "", tr("Scene Files (*.%1)").arg(mn::gl::AbstractScene::extension));
	if (fileName.isEmpty()) {
		return;
	}

	std::ifstream fileStream(fileName.toStdString().c_str());
	if (!fileStream) {
		QMessageBox::critical(this, tr("Error reading file"),
		                      tr("Unable to open file %1").arg(fileName));
		return;
	}

	mn::gl::AbstractScene::ptr scene;
	try {
		scene = mn::gl::AbstractScene::load(fileStream);
	} catch (const mn::lib::Lexer::error &e) {
		QMessageBox::critical(this, tr("Error reading file"),
		                      tr("Parser returned error:\n%1:%2:%3:\n\t%4").arg(fileName).arg(e.location.begin.line).arg(e.location.begin.column).arg(e.what()));
		return;
	}

	pane->gl->setScene(scene);
}

void MainWindow::save()
{
	mn::gl::AbstractScene *sc = pane->gl->getScene();
	if (!sc) {
		QMessageBox::warning(this, tr("No scene to save!"), tr("There's no scene loaded, so there's nothing to save."), QMessageBox::Cancel);
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save Scene"), "", tr("Scene Files (*.%1)").arg(mn::gl::AbstractScene::extension));
	if (fileName.isEmpty()) {
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
	QMenu *fileMenu = ui->menubar->addMenu(tr("&File", "menu"));

	QAction *exitAction = new QAction(tr("E&xit", "exit app"), this);
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

	QAction *saveAction = new QAction(this);
	saveAction->setText(tr("&Save", "action to save file"));
	saveAction->setToolTip("Save scene file");
	saveAction->setShortcut(QKeySequence::Save);
	connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

	QAction *loadAction = new QAction(this);
	loadAction->setText(tr("&Load", "action to load file"));
	loadAction->setToolTip("Open scene file");
	loadAction->setShortcut(QKeySequence::Open);
	connect(loadAction, SIGNAL(triggered()), this, SLOT(load()));

	QAction *settingsAction = new QAction(tr("Settings", "menu"), this);
	settingsAction->setToolTip(tr("Change settings of simulation"));
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

	fileMenu->addAction(settingsAction);
	fileMenu->addSeparator();
	fileMenu->addAction(loadAction);
	fileMenu->addAction(saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);
}
