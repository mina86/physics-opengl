#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <Qt>
#include "../config.hpp"

namespace Ui {
	class SettingsDialog;
}

class SettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = 0);
	explicit SettingsDialog(QWidget *parent, mn::gl::Configuration &config);
	~SettingsDialog();
	mn::gl::Configuration config;
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
	Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_HPP
