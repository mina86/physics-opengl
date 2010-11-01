#ifndef H_CONFIG_HPP
#define H_CONFIG_HPP

#define MAX_DISTANCE 1500.0

#include <qobject.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

namespace mn {

namespace gl {

struct Configuration;

/*
 * Qt is so lame.  When you define a class inside another one it does
 * not handle Q_OBJECT and inheriting from QObject correctly...
 * That's why those classes are in their own namespace rather than
 * inside the Configuration class.
 */
namespace cfg {

struct Bool : public QObject {
	Bool(bool v = false) : value(v) { }

	operator bool() const { return value; }

	Bool &operator=(bool v) {
		set(v);
		return *this;
	}

	Bool &operator=(const Bool &v) {
		set(v.value);
		return *this;
	}

public slots:
	void set(bool v);
	void set() { set(true); }
	void clear() { set(false); }
	void toggle() { set(!value); }

signals:
	void changed(bool v);

private:
	bool value;

	Q_OBJECT
};

struct Real : public QObject  {
	typedef GLfloat value_type;

	Real(value_type v = 0.0) : value(v) { }

	operator value_type() const { return value; }

	Real &operator=(value_type v) {
		set(v);
		return *this;
	}

	Real &operator=(const Real &v) {
		set(v.value);
		return *this;
	}

public slots:
	void set(value_type v);

signals:
	void changed(value_type v);
	void changed();

private:
	value_type value;

	Q_OBJECT
};

struct Quadric : public QObject  {
	Quadric();

	~Quadric();

	GLenum getDrawStyle() const { return drawStyle; }

	operator GLUquadric *() const { return quad; }

public slots:
	void setDrawStyle(GLenum theDrawStyle);
	void setNextDrawStyle();

signals:
	void drawStyleChanged(GLenum drawStyle);

private:
	GLUquadric *quad;
	GLenum drawStyle;

	Q_OBJECT
};

struct Data : public QObject {
	cfg::Real mouseMovementFactor, mouseRotationFactor;
	cfg::Real runFactor, creepFactor;
	cfg::Real cutOffDistance2;
	cfg::Bool showText, showTextures, lowQuality;
	cfg::Bool showHelperAxis, showStars;
	cfg::Quadric quad;

signals:
	void changed();

private slots:
	void somethingChanged();

private:
	Data();
	Data(const Data &);

	unsigned references;

	Q_OBJECT

	friend struct gl::Configuration;
};

}

struct Configuration {
	typedef GLfloat value_type;

	Configuration(Configuration &config) : ptr(config.ptr) {
		++ptr->references;
	}
	Configuration() : ptr(new cfg::Data()) { }
	~Configuration() {
		if (!--ptr->references) {
			delete ptr;
		}
	}

	operator cfg::Data *()              { return  ptr; }
	operator cfg::Data &()              { return *ptr; }
	operator const cfg::Data *()  const { return  ptr; }
	operator const cfg::Data &()  const { return *ptr; }
	cfg::Data       &operator*()        { return *ptr; }
	cfg::Data       *operator->()       { return  ptr; }
	cfg::Data       *operator&()        { return  ptr; }
	const cfg::Data &operator*()  const { return *ptr; }
	const cfg::Data *operator->() const { return  ptr; }
	const cfg::Data *operator&()  const { return  ptr; }

private:
	Configuration(const Configuration &);

	cfg::Data *ptr;
};

}

}

#endif
