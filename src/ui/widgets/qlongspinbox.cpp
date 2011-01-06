#include "qlongspinbox.hpp"

#include <QLineEdit>
#include <iostream>

namespace ui {

QLongSpinBox::QLongSpinBox(QWidget *parent) :
	QAbstractSpinBox(parent),
	value(0), min(0), max(99), step(1)
{
	connect(this, SIGNAL(editingFinished()), this, SLOT(doInterpretation()));
	updateText();
}

void QLongSpinBox::setValue(long newValue)
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

void QLongSpinBox::updateText()
{
	lineEdit()->setText(QString("%1").arg(value));
}

void QLongSpinBox::stepBy(int steps)
{
	setValue(value + steps * step);
}

QAbstractSpinBox::StepEnabled QLongSpinBox::stepEnabled() const
{
	QAbstractSpinBox::StepEnabled flags;
	if (value > min)
		flags |= StepDownEnabled;
	if (value < max)
		flags |= StepUpEnabled;
	return flags;
}

void QLongSpinBox::setMinimum(long newMin)
{
	min = newMin;
}

void QLongSpinBox::setMaximum(long newMax)
{
	max = newMax;
}

void QLongSpinBox::setRange(long min, long max)
{
	setMinimum(min);
	setMaximum(max);
}

void QLongSpinBox::setStep(long newStep)
{
	step = newStep;
}

QValidator::State QLongSpinBox::validate(QString &input, int &) const
{
	bool ok;
	input.toLong(&ok, 10);
	if (ok)
		return QValidator::Acceptable;
	else if (input.isEmpty())
		return QValidator::Intermediate;
	else
		return QValidator::Invalid;
}

void QLongSpinBox::fixup(QString &input) const
{
	input = QString("%1").arg(value);
}

void QLongSpinBox::doInterpretation()
{
	bool ok;
	long v = text().toLong(&ok, 10);
	if (ok)
		setValue(v);
}

}

