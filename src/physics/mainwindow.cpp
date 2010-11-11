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
	initActions();
}

MainWindow::MainWindow(mn::gl::Configuration &config) :
		QMainWindow(0),
		config(config),
		ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	initActions();
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

void MainWindow::openSettingsDialog()
{
	SettingsDialog *settingsDialog = new SettingsDialog(this, config);
	settingsDialog->show();
}

void MainWindow::load()
{
	QStringList filters;
	filters << tr("Scene files (*.%1)").arg(mn::gl::AbstractScene::extension)
			<< tr("Any file (*)");

	QFileDialog dialog(this);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setDefaultSuffix(mn::gl::AbstractScene::extension);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilters(filters);
	dialog.setWindowTitle(tr("Load Scene"));
	if (dialog.exec() == QDialog::Rejected)
		return;

	QString fileName = dialog.selectedFiles().front();
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

	QStringList filters;
	filters << tr("Scene files (*.%1)").arg(mn::gl::AbstractScene::extension)
			<< tr("Any file (*)");

	QFileDialog dialog(this);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setConfirmOverwrite(true);
	dialog.setDefaultSuffix(mn::gl::AbstractScene::extension);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilters(filters);
	dialog.setWindowTitle(tr("Save Scene"));
	if (dialog.exec() == QDialog::Rejected)
		return;

	QString fileName = dialog.selectedFiles().front();
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
	fileMenu = ui->menubar->addMenu(tr("&File", "menu"));

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

	onWidgetSceneChanged();
}

void MainWindow::onWidgetSceneChanged()
{
	saveAction->setEnabled(pane && pane->gl && pane->gl->getScene());
}
