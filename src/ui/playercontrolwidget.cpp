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
		fpsRate(0)
{
	ui.setupUi(this);
	mTimer = new QTimer(this);
	mTimer->setSingleShot(false);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(playNextFrame()));

	connect(ui.playButton, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui.pauseButton, SIGNAL(clicked()), this, SLOT(pause()));

	connect(ui.fpsSlider, SIGNAL(valueChanged(int)), this, SLOT(setFps(int)));
	connect(ui.fpsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setFps(int)));
	connect(this, SIGNAL(fpsChanged(int)), ui.fpsSlider, SLOT(setValue(int)));
	connect(this, SIGNAL(fpsChanged(int)), ui.fpsSpinBox, SLOT(setValue(int)));
	connect(ui.speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
	connect(ui.speedSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setSpeed(double)));
	connect(ui.precisionSlider, SIGNAL(valueChanged(int)), this, SLOT(setPrecision(int)));
	connect(ui.precisionSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setPrecision(double)));

	ui.playButton->setEnabled(fpsRate > 0 && !isPlaying);
	ui.pauseButton->setEnabled(isPlaying);

	fpsRate = -1000;
	speedExponent = -1000;
	precisionExponent = -1000;
	setFps(0);
	setSpeed(0.0);
	setPrecision(0.0);
	debugprint();
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
	if (play == isPlaying)
		return;

	isPlaying = play;
	ui.playButton->setEnabled(!isPlaying && fpsRate > 0);
	ui.pauseButton->setEnabled(isPlaying);
	if (isPlaying && fpsRate > 0)
		mTimer->start();
	else
		mTimer->stop();
}

void PlayerControlWidget::setFps(int newFps)
{
	if (newFps == fpsRate)
		return;

	fpsRate = newFps;
	emit fpsChanged(fpsRate);
	if (fpsRate == 0)
	{
		pause();
		ui.playButton->setEnabled(false);
	}
	else
	{
		ui.playButton->setEnabled(!isPlaying);
		mTimer->setInterval(1000 / fpsRate);
	}
	debugprint();
}

void PlayerControlWidget::playNextFrame()
{
	float rate = fpsRate ? 1/fpsRate : 1.0;
	float tickCount = (speed/precision)/rate;
	emit newFrameNeeded(std::min((unsigned)1, (unsigned)round(tickCount)), precision);
}

void PlayerControlWidget::debugprint()
{
	ui.fpsDisplayLabel->setText(QString(tr("FPS: %1; new frame each %2 seconds"))
				 .arg(fpsRate, 3)
				 .arg((fpsRate == 0 ? 0 : 1.0/(double)fpsRate), 8, 'g', 4));
	ui.speedDisplayLabel->setText(QString(tr("Speed: %1 (%2x)"))
				 .arg(speedExponent)
				 .arg(speed));
	ui.precisionDisplayLabel->setText(QString(tr("Pracision: %1 (%2)"))
				 .arg(precisionExponent)
				 .arg(precision));
}

void PlayerControlWidget::setSpeed(int newSpeed)
{
	setSpeed((double)newSpeed / 10.0);
}

void PlayerControlWidget::setSpeed(double newSpeed)
{
	if (fabs(newSpeed - speedExponent) < FLOATCOMPAREEPSILON)
		return;

	speedExponent = newSpeed;
	speed = pow10(speedExponent);
	ui.speedSlider->setValue(round(speedExponent * 10));
	ui.speedSpinBox->setValue(speedExponent);
	debugprint();
}

void PlayerControlWidget::setPrecision(int newPrecision)
{
	setPrecision((double)newPrecision / 10.0);
}

void PlayerControlWidget::setPrecision(double newPrecision)
{
	if (fabs(newPrecision - precisionExponent) < FLOATCOMPAREEPSILON)
		return;

	precisionExponent = newPrecision;
	precision = pow10(precisionExponent);
	ui.precisionSlider->setValue(round(precisionExponent * 10));
	ui.precisionSpinBox->setValue(precisionExponent);
	debugprint();
}

}
