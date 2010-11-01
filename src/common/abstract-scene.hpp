#ifndef H_ABSTRACT_OBJECTS_HPP
#define H_ABSTRACT_OBJECTS_HPP

#include <memory>

namespace mn {

namespace gl {

struct Widget;

struct AbstractScene {
	typedef std::auto_ptr<AbstractScene> ptr;

	virtual ~AbstractScene() { }
	virtual void initializeGL() = 0;
	virtual void drawScene(const Widget &gl) = 0;
	virtual void updateState(unsigned ticks, float dt) = 0;
};

}

}

#endif
