#include "qfloatspinbox.hpp"

#include <QLineEdit>
#include <iostream>

namespace ui {

QFloatSpinBox::QFloatSpinBox(QWidget *parent) :
	QAbstractSpinBox(parent),
	value(0), min(0), max(99), step(1)
{
	connect(this, SIGNAL(editingFinished()), this, SLOT(doInterpretation()));
	updateText();
}

void QFloatSpinBox::setValue(float newValue)
{
	if (newValue == value)
		return;

	if (newValue > max)
		newValue = max;
	if (newValue < min)
		newValue = min;
	value = newValue;
	emit valueChanged();
	emit valueChanged(value);
	updateText();
}

void QFloatSpinBox::updateText()
{
	lineEdit()->setText(QString("%1").arg(value));
}

void QFloatSpinBox::stepBy(int steps)
{
	setValue(value + steps * step);
}

QAbstractSpinBox::StepEnabled QFloatSpinBox::stepEnabled() const
{
	QAbstractSpinBox::StepEnabled flags;
	if (value > min)
		flags |= StepDownEnabled;
	if (value < max)
		flags |= StepUpEnabled;
	return flags;
}

void QFloatSpinBox::setMinimum(float newMin)
{
	min = newMin;
}

void QFloatSpinBox::setMaximum(float newMax)
{
	max = newMax;
}

void QFloatSpinBox::setRange(float min, float max)
{
	setMinimum(min);
	setMaximum(max);
}

void QFloatSpinBox::setStep(float newStep)
{
	step = newStep;
}

QValidator::State QFloatSpinBox::validate(QString &input, int &) const
{
	bool ok;
	input.toFloat(&ok);
	if (ok)
		return QValidator::Acceptable;
	else if (input.isEmpty())
		return QValidator::Intermediate;
	else
		return QValidator::Invalid;
}

void QFloatSpinBox::fixup(QString &input) const
{
	input = QString("%1").arg(value);
}

void QFloatSpinBox::doInterpretation()
{
	bool ok;
	float v = text().toFloat(&ok);
	if (ok)
		setValue(v);
}

}

