#include "autosettingsdialog.hpp"

#include <QFormLayout>

namespace ui {

AutoSettingsDialog::AutoSettingsDialog(cfg::Data *data, QWidget *parent, Qt::WindowFlags f) :
	QDialog(parent, f)
{
	QFormLayout *layout = new QFormLayout(this);

	for (cfg::Data::iterator it = data->items(); it; ++it) {
		cfg::Value *valueItem = &(it(data));
		layout->addRow(valueItem->name, valueItem->makeControlWidget(this));
	}

	setLayout(layout);
}

}

