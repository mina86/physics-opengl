#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "../gl/config.hpp"
#include "../ui/glpane.hpp"

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
	void setPane(mn::ui::GLPane *thePane) {
		pane = thePane;
		setCentralWidget(pane);
	}

protected:
	void changeEvent(QEvent *e);
	void prepare();

	mn::ui::GLPane *pane;

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_HPP
