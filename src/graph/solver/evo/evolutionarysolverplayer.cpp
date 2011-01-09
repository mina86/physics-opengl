#include "evolutionarysolverplayer.hpp"

namespace graph {

namespace solver {

namespace evo {

EvolutionarySolverPlayer::EvolutionarySolverPlayer(QWidget *parent) :
QWidget(parent)
{
	ui.setupUi(this);
}

void EvolutionarySolverPlayer::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui.retranslateUi(this);
		break;
	default:
		break;
	}
}

}

}

}
