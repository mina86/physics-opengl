/*
 * src/ui/dialogs/settingsdialog.cpp
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
#include "settingsdialog.hpp"

namespace ui {

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
	ui.setupUi(this);
}

SettingsDialog::SettingsDialog(QWidget *parent, gl::Config &config)
	: QDialog(parent), config(config) {
	ui.setupUi(this);
}

void SettingsDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui.retranslateUi(this);
		break;
	default:
		break;
	}
}

void SettingsDialog::show()
{
	ui.checkBoxShowText->setCheckState(config->showText ? Qt::Checked : Qt::Unchecked);
	ui.checkBoxShowTextures->setCheckState(config->showTextures ? Qt::Checked : Qt::Unchecked);
	ui.checkBoxShowAxes->setCheckState(config->showHelperAxis ? Qt::Checked : Qt::Unchecked);
	ui.checkBoxShowStars->setCheckState(config->showStars ? Qt::Checked : Qt::Unchecked);
	ui.checkBoxLowQuality->setCheckState(config->lowQuality ? Qt::Checked : Qt::Unchecked);
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

}
