#ifndef AUTOSETTINGSDIALOG_HPP
#define AUTOSETTINGSDIALOG_HPP

#include <QDialog>

#include "../abstract-config.hpp"

namespace ui {

struct AutoSettingsDialog : public QDialog
{
	AutoSettingsDialog(cfg::Data *data, QWidget *parent = 0, Qt::WindowFlags f = 0, QString buttonText = tr("Clo&se", "Autoconfigdialog button text"));

signals:

public slots:

private:
	Q_OBJECT
};

}

#endif // AUTOSETTINGSDIALOG_HPP
