#ifndef PLAYERCONTROLWIDGET_HPP
#define PLAYERCONTROLWIDGET_HPP

#include <QWidget>
#include <QTimer>

namespace Ui {
	class PlayerControlWidget;
}

class PlayerControlWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PlayerControlWidget(QWidget *parent = 0);
	~PlayerControlWidget();

public slots:
	void play();
	void pause();
	void togglePlay();
	void setPlay(bool play);
	void setSpeed(int newSpeed);

signals:
	void speedChanged(int);
	void nextFramePlayed();

protected:
	void changeEvent(QEvent *e);
	QTimer *mTimer;
	bool isPlaying;
	int speed;

protected slots:
	void playNextFrame();

private:
	Ui::PlayerControlWidget *ui;
};

#endif // PLAYERCONTROLWIDGET_HPP
