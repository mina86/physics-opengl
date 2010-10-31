#ifndef H_CONFIG_HPP
#define H_CONFIG_HPP

#include <qobject.h>

#ifdef __APPLE__
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif

namespace mn {

namespace gl {

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

}

struct Configuration : public QObject {
	typedef GLfloat value_type;

	struct ptr {
		ptr(Configuration &theConfig) : config(theConfig) { config.get(); }
		ptr(Configuration *theConfig) : config(*theConfig) { config.get(); }
		ptr() : config(*new Configuration()) { }
		~ptr() {
			config.put();
		}

		operator Configuration *()              { return &config; }
		operator Configuration &()              { return  config; }
		operator const Configuration *()  const { return &config; }
		operator const Configuration &()  const { return  config; }
		Configuration       &operator*()        { return  config; }
		Configuration       *operator->()       { return &config; }
		Configuration       *operator&()        { return &config; }
		const Configuration &operator*()  const { return  config; }
		const Configuration *operator->() const { return &config; }
		const Configuration *operator&()  const { return &config; }

	private:
		Configuration &config;
	};

	void get() { ++references; }
	void put() {
		if (!--references) {
			delete this;
		}
	}

	static const value_type maxDistance = 1500.0;

	cfg::Real mouseMovementFactor, mouseRotationFactor;
	cfg::Real runFactor, creepFactor;
	cfg::Real cutOffDistance2;
	cfg::Bool showText, showTextures, lowQuality, showHelperAxis;

	cfg::Quadric quad;

signals:
	void changed();

private slots:
	void somethingChanged();

private:
	Configuration();
	Configuration(const Configuration &);
	void operator=(const Configuration &);

	unsigned references;

	Q_OBJECT
};

}

}

#endif
