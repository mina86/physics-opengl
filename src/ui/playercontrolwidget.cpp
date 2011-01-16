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

#include "playercontrolwidget.hpp"

#include <cmath>

namespace ui {

PlayerControlWidget::PlayerControlWidget(QWidget *parent) :
		QWidget(parent),
		isPlaying(false),
		tickCountOverflow(0)
{
	ui.setupUi(this);
	mTimer = new QTimer(this);
	mTimer->setSingleShot(false);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(playNextFrame()));

	connect(ui.playButton, SIGNAL(clicked()), this, SLOT(togglePlay()));
	connect(ui.stepButton, SIGNAL(clicked()), this, SLOT(step()));

	connect(ui.fpsSlider, SIGNAL(valueChanged(int)), this, SLOT(setFps(int)));
	connect(ui.fpsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setFps(int)));
	connect(this, SIGNAL(fpsChanged(int)), ui.fpsSlider, SLOT(setValue(int)));
	connect(this, SIGNAL(fpsChanged(int)), ui.fpsSpinBox, SLOT(setValue(int)));
	connect(ui.speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
	connect(ui.speedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));

	fpsRate = 0;
	setFps(1);
	speed = 0;
	setSpeed(1);

	ui.playButton->setText(tr("&Play"));
	ui.stepButton->setEnabled(true);
}

void PlayerControlWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui.retranslateUi(this);
		break;
	default:
		break;
	}
}

void PlayerControlWidget::play()
{
	setPlay(true);
}

void PlayerControlWidget::pause()
{
	setPlay(false);
}

void PlayerControlWidget::togglePlay()
{
	setPlay(!isPlaying);
}

void PlayerControlWidget::setPlay(bool play)
{
	if (!fpsRate) {
		play = false;
	}

	if (play == isPlaying)
		return;

	isPlaying = play;
	ui.stepButton->setEnabled(!isPlaying);
	if (isPlaying) {
		ui.playButton->setText(tr("&Pause"));
		mTimer->start();
	} else {
		ui.playButton->setText(tr("&Play"));
		mTimer->stop();
	}
}

void PlayerControlWidget::setFps(int newFps)
{
	if (newFps == fpsRate)
		return;

	fpsRate = newFps;
	if (!fpsRate) {
		pause();
	} else {
		mTimer->setInterval(1000 / fpsRate);
	}

	emit fpsChanged(fpsRate);
}

void PlayerControlWidget::playNextFrame()
{
	float tickCount = speed / fpsRate + tickCountOverflow;
	tickCountOverflow = tickCount - floor(tickCount);
	unsigned ticks = floor(tickCount);
	if (ticks) {
		emit newFrameNeeded(ticks);
	}
}

void PlayerControlWidget::step()
{
	if (!isPlaying) {
		emit newFrameNeeded(1);
	}
}

void PlayerControlWidget::setSpeed(int newSpeed)
{
	if ((int)speed != newSpeed) {
		speed = newSpeed;
		ui.speedSlider->setValue(newSpeed);
		ui.speedSpinBox->setValue(newSpeed);
	}
}

}
