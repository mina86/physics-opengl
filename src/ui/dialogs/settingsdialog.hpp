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
#ifndef H_SETTINGSDIALOG_HPP
#define H_SETTINGSDIALOG_HPP

#include <QDialog>
#include <Qt>

#include "../../gl/glconfig.hpp"

#include "ui_settingsdialog.h"

namespace ui {

struct SettingsDialog : public QDialog {
	explicit SettingsDialog(QWidget *parent = 0);
	explicit SettingsDialog(QWidget *parent, gl::Config &config);
	gl::Config config;
	void show();

public slots:
	void on_checkBoxShowText_stateChanged(int state);
	void on_checkBoxShowTextures_stateChanged(int state);
	void on_checkBoxShowAxes_stateChanged(int state);
	void on_checkBoxShowStars_stateChanged(int state);
	void on_checkBoxLowQuality_stateChanged(int state);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::SettingsDialog ui;

	Q_OBJECT
};

}

#endif
