#ifndef PLAYERCONTROLWIDGET_HPP
#define PLAYERCONTROLWIDGET_HPP

#define FLOATCOMPAREEPSILON 0.0001

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
	void setFps(int newSpeed);

	void setSpeed(int);
	void setSpeed(double);
	void setPrecision(int);
	void setPrecision(double);

signals:
	void fpsChanged(int);
	void nextFramePlayed();

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

protected slots:
	void playNextFrame();

private:
	Ui::PlayerControlWidget *ui;
};

#endif // PLAYERCONTROLWIDGET_HPP
