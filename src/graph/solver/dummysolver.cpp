#include "dummysolver.hpp"

#include <QPushButton>

#include "../vector-rand.hpp"

namespace graph
{

DummySolver::DummySolver(QObject *parent) :
	AbstractSolver(parent)
{
}

QWidget* DummySolver::createPlayerWidget(QWidget *theParent)
{
	QPushButton* button = new QPushButton(tr("Play once"), theParent);
	connect(button, SIGNAL(clicked()), this, SLOT(playOnce()));
	return button;
}

void DummySolver::playOnce()
{
	Scene::nodes_iterator i;
	for (i = scene->nodes_begin(); i != scene->nodes_end(); ++i)
	{
		Scene::node_type n = *i;
		addRandNormal(n.first, 0.03);
	}

	emit oneStepMade();
}

}
