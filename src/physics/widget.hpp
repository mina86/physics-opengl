#ifndef H_WIDGET_HPP
#define H_WIDGET_HPP

#include <new>

#include "../common/glwidget.hpp"
#include "../common/glpane.hpp"
#include "../common/config.hpp"

namespace mn {

namespace physics {

struct Object;

struct PhysicsWidget : public ui::GLWidget {
	PhysicsWidget(Object *theObjects, gl::Configuration &cfg,
	              QWidget *parent = NULL);
	~PhysicsWidget();

protected:
	virtual void initializeGL();
	virtual void doPaint();

private:
	Object *objects;
};

}

}

#endif
