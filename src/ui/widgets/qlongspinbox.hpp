#ifndef QLONGSPINBOX_HPP
#define QLONGSPINBOX_HPP

#include <QAbstractSpinBox>

namespace ui {

class QLongSpinBox : public QAbstractSpinBox
{
	Q_OBJECT
public:
	explicit QLongSpinBox(QWidget *parent = 0);
	void stepBy(int steps);
	QValidator::State validate(QString &input, int &pos) const;
	void fixup(QString &input) const;

protected:
	StepEnabled stepEnabled() const;

signals:
	void valueChanged();
	void valueChanged(long value);

public slots:
	void setValue(long value);
	void setMinimum(long min);
	void setMaximum(long max);
	void setRange(long min, long max);
	void setStep(long step);

private:
	long value;
	long min, max, step;
	void updateText();

private slots:
	void doInterpretation();

};

}

#endif // QLONGSPINBOX_HPP
