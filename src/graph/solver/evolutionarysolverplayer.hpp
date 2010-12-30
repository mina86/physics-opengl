#ifndef EVOLUTIONARYSOLVERPLAYER_HPP
#define EVOLUTIONARYSOLVERPLAYER_HPP

#include <QWidget>
#include "ui_evolutionarysolverplayer.h"

namespace Ui {
	class EvolutionarySolverPlayer;
}

namespace graph {

class EvolutionarySolverPlayer : public QWidget
{
	Q_OBJECT

public:
	explicit EvolutionarySolverPlayer(QWidget *parent = 0);

protected:
	void changeEvent(QEvent *e);

private:
	Ui::EvolutionarySolverPlayer ui;

friend class EvolutionarySolver;

};

}

#endif // EVOLUTIONARYSOLVERPLAYER_HPP
