#include "autosettingsdialog.hpp"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

namespace ui {

AutoSettingsDialog::AutoSettingsDialog(cfg::Data *data, QWidget *parent, Qt::WindowFlags f) :
	QDialog(parent, f)
{
	QVBoxLayout *dialogLayout = new QVBoxLayout(this);

	QWidget *optionsWidget = new QWidget(this);
	QFormLayout *layout = new QFormLayout(optionsWidget);
	for (cfg::Data::iterator it = data->items(); it; ++it) {
		cfg::Value *valueItem = &(it(data));
		layout->addRow(valueItem->name, valueItem->makeControlWidget(optionsWidget));
	}
	optionsWidget->setLayout(layout);
	dialogLayout->addWidget(optionsWidget);

	QPushButton *closeButton = new QPushButton(tr("Clo&se"), this);
	closeButton->setDefault(true);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	QDialogButtonBox *bbox = new QDialogButtonBox(Qt::Horizontal, this);
	bbox->addButton(closeButton, QDialogButtonBox::YesRole);
	dialogLayout->addWidget(bbox);

	setLayout(dialogLayout);
}

}

