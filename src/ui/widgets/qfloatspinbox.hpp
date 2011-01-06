#ifndef QFLOATSPINBOX_HPP
#define QFLOATSPINBOX_HPP

#include <QAbstractSpinBox>

namespace ui {

class QFloatSpinBox : public QAbstractSpinBox
{
	Q_OBJECT
public:
	explicit QFloatSpinBox(QWidget *parent = 0);
	void stepBy(int steps);
	QValidator::State validate(QString &input, int &pos) const;
	void fixup(QString &input) const;

protected:
	StepEnabled stepEnabled() const;

signals:
	void valueChanged();
	void valueChanged(float value);

public slots:
	void setValue(float value);
	void setMinimum(float min);
	void setMaximum(float max);
	void setRange(float min, float max);
	void setStep(float step);

private:
	float value;
	float min, max, step;
	void updateText();

private slots:
	void doInterpretation();

};

}

#endif // QFLOATSPINBOX_HPP
