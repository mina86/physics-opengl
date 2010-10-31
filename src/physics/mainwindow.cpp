#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "../common/dialogs/settingsdialog.hpp"

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	prepare();
}

MainWindow::MainWindow(mn::gl::Configuration::ptr &config) :
		QMainWindow(0),
		config(config),
		ui(new Ui::MainWindow)
{
	ui->setupUi(this);
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
