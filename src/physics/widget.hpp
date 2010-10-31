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
	struct Factory : public ui::GLPane::GLWidgetFactory {
		Factory(Object *theObjects, gl::Configuration &cfg)
			: GLWidgetFactory(cfg), objects(theObjects) {
			if (!theObjects) {
				throw std::bad_alloc();
			}
		}
		~Factory();

	protected:
		GLWidget *create(gl::Configuration &theConfig, QWidget *parent);

	private:
		Object *objects;
	};

	PhysicsWidget(Object *theObjects, gl::Configuration &cfg,
	              QWidget *parent = NULL);
	~PhysicsWidget();

protected:
	virtual void initializeGL();
	virtual void doPaint();

private:
	Object *objects;

	static void deleteObjects(Object *objects);
};

}

}

#endif
