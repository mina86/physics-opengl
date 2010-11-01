#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "../common/config.hpp"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	MainWindow(mn::gl::Configuration &config);
	~MainWindow();
	mn::gl::Configuration config;

public slots:
	void openSettingsDialog();

protected:
	void changeEvent(QEvent *e);
	void prepare();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
