#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "../gl/config.hpp"
#include "../ui/glpane.hpp"
#include "../gl/glwidget.hpp"

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
		connect(pane->gl, SIGNAL(sceneChanged()), this, SLOT(onWidgetSceneChanged()));
	}
	void load();
	void save();

protected:
	void changeEvent(QEvent *e);
	void prepare();

	mn::ui::GLPane *pane;

protected slots:
	void onWidgetSceneChanged();

private:
	Ui::MainWindow *ui;
	void initActions();

	QMenu *fileMenu;
	QAction *saveAction;
	QAction *loadAction;
	QAction *quitAction;
	QAction *settingsAction;
};

#endif // MAINWINDOW_HPP
