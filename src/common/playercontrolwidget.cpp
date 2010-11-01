#include "playercontrolwidget.hpp"
#include "ui_playercontrolwidget.h"

PlayerControlWidget::PlayerControlWidget(QWidget *parent) :
		QWidget(parent),
		isPlaying(false),
		speed(0),
		ui(new Ui::PlayerControlWidget)
{
	ui->setupUi(this);
	mTimer = new QTimer(this);
	mTimer->setSingleShot(false);
	connect(mTimer, SIGNAL(timeout()), this, SLOT(playNextFrame()));

	connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
	connect(ui->speedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setSpeed(int)));
	connect(this, SIGNAL(speedChanged(int)), ui->speedSlider, SLOT(setValue(int)));
	connect(this, SIGNAL(speedChanged(int)), ui->speedSpinBox, SLOT(setValue(int)));

	connect(ui->playButton, SIGNAL(clicked()), this, SLOT(play()));
	connect(ui->pauseButton, SIGNAL(clicked()), this, SLOT(pause()));

	ui->playButton->setEnabled(speed > 0 && !isPlaying);
	ui->pauseButton->setEnabled(isPlaying);
}

PlayerControlWidget::~PlayerControlWidget()
{
	delete ui;
}

void PlayerControlWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
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
	ui->playButton->setEnabled(!isPlaying && speed > 0);
	ui->pauseButton->setEnabled(isPlaying);
	if (isPlaying && speed > 0)
		mTimer->start();
	else
		mTimer->stop();
}

void PlayerControlWidget::setSpeed(int newSpeed)
{
	if (newSpeed == speed)
		return;
	speed = newSpeed;
	emit speedChanged(speed);
	if (speed == 0)
	{
		pause();
		ui->playButton->setEnabled(false);
	}
	else
	{
		ui->playButton->setEnabled(!isPlaying);
		mTimer->setInterval(1000 / speed);
	}
}

void PlayerControlWidget::playNextFrame()
{
	emit nextFramePlayed();
}
