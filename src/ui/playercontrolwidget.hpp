/*
 * src/ui/playercontrolwidget.hpp
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
#ifndef H_PLAYERCONTROLWIDGET_HPP
#define H_PLAYERCONTROLWIDGET_HPP

#define FLOATCOMPAREEPSILON 0.0001

#include <QWidget>
#include <QTimer>

#include "ui_playercontrolwidget.h"

namespace ui {

struct PlayerControlWidget : public QWidget {
	explicit PlayerControlWidget(QWidget *parent = 0);

public slots:
	void play();
	void pause();
	void togglePlay();
	void setPlay(bool play);
	void setFps(int newSpeed);

	void setSpeed(int);
	void setSpeed(double);
	void setPrecision(int);
	void setPrecision(double);

signals:
	void fpsChanged(int);
	void newFrameNeeded(unsigned tickCount, float tickInterval);

protected:
	void changeEvent(QEvent *e);
	void debugprint();

	QTimer *mTimer;
	bool isPlaying;

	int fpsRate;
	double speed;
	double speedExponent;
	double precision;
	double precisionExponent;

	double tickCountOverflow;

protected slots:
	void playNextFrame();

private:
	Ui::PlayerControlWidget ui;

	Q_OBJECT
};

}

#endif
