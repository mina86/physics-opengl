#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
}

SettingsDialog::SettingsDialog(QWidget *parent, mn::gl::Configuration::ptr &config) :
		QDialog(parent),
		config(config),
		ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
	delete ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SettingsDialog::show()
{
	ui->checkBoxShowText->setCheckState(config->showText ? Qt::Checked : Qt::Unchecked);
	ui->checkBoxShowTextures->setCheckState(config->showTextures ? Qt::Checked : Qt::Unchecked);
	ui->checkBoxShowAxes->setCheckState(config->showHelperAxis ? Qt::Checked : Qt::Unchecked);
	ui->checkBoxShowStars->setCheckState(config->showStars ? Qt::Checked : Qt::Unchecked);
	ui->checkBoxLowQuality->setCheckState(config->lowQuality ? Qt::Checked : Qt::Unchecked);
	QDialog::show();
}

void SettingsDialog::on_checkBoxShowText_stateChanged(int state)
{
	config->showText.set(state == Qt::Checked);
}

void SettingsDialog::on_checkBoxShowTextures_stateChanged(int state)
{
	config->showTextures.set(state == Qt::Checked);
}

void SettingsDialog::on_checkBoxShowAxes_stateChanged(int state)
{
	config->showHelperAxis.set(state == Qt::Checked);
}

void SettingsDialog::on_checkBoxShowStars_stateChanged(int state)
{
	config->showStars.set(state == Qt::Checked);
}
void SettingsDialog::on_checkBoxLowQuality_stateChanged(int state)
{
	config->lowQuality.set(state == Qt::Checked);
}
