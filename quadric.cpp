#include "quadric.hpp"

namespace mn {

namespace gl {


Quadric *Quadric::singleton = 0;


void Quadric::setNextDrawStyle() {
	GLenum ds;
	switch (drawStyle) {
	case GLU_FILL: ds = GLU_LINE; break;
	case GLU_LINE: ds = GLU_POINT; break;
	default: ds = GLU_FILL; break;
	}
	setDrawStyle(ds);
}


}

}
