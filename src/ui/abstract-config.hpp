/*
 * src/gl/config.hpp
 * Copyright 2010 by Michal Nazarewicz    <mina86@mina86.com>
 *               and Maciej Swietochowski <m@swietochowski.eu>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef H_CONFIG_HPP
#define H_CONFIG_HPP

#include <stdexcept>

#include <QObject>

namespace ui {

template<class Data>
struct Config;

/*
 * Qt is so lame.  When you define a class inside another one it does
 * not handle Q_OBJECT and inheriting from QObject correctly...
 * That's why those classes are in their own namespace rather than
 * inside the AbstractConfig class.
 */
namespace cfg {

struct Bool;
struct Integer;
struct Real;
struct List;

struct Value : public QObject {
	virtual ~Value();

	const char *const name;
	const enum Type { BOOL, INTEGER, REAL, LIST } type;

	template<class T>
	T *cast() { return static_cast<T*>(this); }
	template<class T>
	const T *cast() const { return static_cast<T*>(this); }

signals:
	void changed(const Value &v);

protected:
	Value(const char *theName, const Bool *) : name(theName), type(BOOL) { }
	Value(const char *theName, const Integer *)
		: name(theName), type(INTEGER) { }
	Value(const char *theName, const Real *) : name(theName), type(REAL) { }
	Value(const char *theName, const List *) : name(theName), type(LIST) { }

private:
	Value();

	Q_OBJECT
};

struct Bool : public Value {
	Bool(const char *theName, bool v = false)
		: Value(theName, this), value(v) { }

	operator bool() const { return value; }
	bool operator!() const { return !value; }

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
	Bool();

	bool value;

	Q_OBJECT
};

struct Integer : public Value  {
	typedef long value_type;

	Integer(const char *theName,
	        value_type theMin, value_type theMax, value_type v)
		throw(std::out_of_range);
	Integer(const char *theName,
	        value_type theMin, value_type theMax)
		throw(std::out_of_range);

	operator value_type() const { return value; }

	Integer &operator=(value_type v) throw(std::out_of_range) {
		set(v);
		return *this;
	}

	Integer &operator=(const Integer &v) throw(std::out_of_range) {
		set(v.value);
		return *this;
	}

	const value_type min, max;

public slots:
	void set(long v) throw(std::out_of_range);

signals:
	void changed(long v);

private:
	Integer();

	value_type value;

	Q_OBJECT
};

struct Real : public Value  {
	typedef float value_type;

	Real(const char *theName,
	     value_type theMin, value_type theMax, value_type v)
		throw(std::out_of_range);
	Real(const char *theName, value_type theMin, value_type theMax)
		throw(std::out_of_range);

	operator value_type() const { return value; }

	Real &operator=(value_type v) throw(std::out_of_range) {
		set(v);
		return *this;
	}

	Real &operator=(const Real &v) throw(std::out_of_range) {
		set(v.value);
		return *this;
	}

	const value_type min, max;

public slots:
	void set(float v) throw(std::out_of_range);

signals:
	void changed(float v);

private:
	Real();

	value_type value;

	Q_OBJECT
};

struct List : public Integer {
	typedef const char *Item;

	List(const char *theName, const Item *theItems, value_type v = 0)
		throw(std::out_of_range);

	const Item *const items;

private:
	Q_OBJECT
};

struct Data : public QObject {
	virtual ~Data();

	struct iterator {
		iterator(const unsigned *offsets) : ptr(offsets) { }

		iterator &operator++()    { ++ptr; return *this; }
		iterator  operator++(int) { return iterator(ptr++); }
		iterator &operator--()    { --ptr; return *this; }
		iterator  operator--(int) { return iterator(ptr--); }

		bool operator==(iterator &it) { return ptr == it.ptr; }
		bool operator!=(iterator &it) { return ptr != it.ptr; }

		operator bool()  const { return *ptr != ~0u; }
		bool operator!() const { return *ptr == ~0u; }

		/*
		 * Unfortunatelly C++ (or GCC?) is lame.  You cannot convert
		 * a pointer to member field of type Foo to a pointer to
		 * member field of type that Foo inherits from.  Because of
		 * this, we use more direct approach with offsets.
		 *
		 * It is dirty, but it should work... at least on GCC. ;)
		 */
		const Value &operator()(const Data *data) const {
			return *reinterpret_cast<const Value *>(reinterpret_cast<const char *>(data) + *ptr);
		}
		Value &operator()(Data *data) const {
			return *reinterpret_cast<Value *>(reinterpret_cast<char *>(data) + *ptr);
		}

#define CFG_DATA_OFFSET(value) \
	(reinterpret_cast<const char *>(static_cast<const ui::cfg::Value *>(&this->value)) - reinterpret_cast<const char *>(this))

	private:
		iterator();
		const unsigned *ptr;
	};

	virtual struct iterator items() const = 0;

signals:
	void changed();

private slots:
	void valueChanged(const Value &v);

protected:
	Data() : references(0) { }
	void init();

private:
	Data(const Data &);

	void get() { ++references; }
	void put() {
		if (!--references) {
			delete this;
		}
	}

	unsigned references;

	Q_OBJECT

	friend struct ui::Config<ui::cfg::Data>;
};

}

template<>
struct Config<cfg::Data> {
	typedef cfg::Data value_type;

	operator cfg::Data *()              { return  get(); }
	operator cfg::Data &()              { return *get(); }
	operator const cfg::Data *()  const { return  get(); }
	operator const cfg::Data &()  const { return *get(); }
	cfg::Data       &operator*()        { return *get(); }
	cfg::Data       *operator->()       { return  get(); }
	cfg::Data       *operator&()        { return  get(); }
	const cfg::Data &operator*()  const { return *get(); }
	const cfg::Data *operator->() const { return  get(); }
	const cfg::Data *operator&()  const { return  get(); }

protected:
	Config(Config &config) : ptr(config.ptr) { ptr->get(); }
	Config(cfg::Data *data) : ptr(data) { ptr->get(); }
	~Config() { ptr->put(); }

	cfg::Data       *get()       { return ptr; }
	const cfg::Data *get() const { return ptr; }

	void set(Config &config) {
		if (ptr != config.ptr) {
			ptr->put();
			config.ptr->get();
			ptr = config.ptr;
		}
	}

private:
	Config();

	cfg::Data *ptr;
};

template<class T>
struct Config : public Config<cfg::Data> {
	typedef T value_type;

	Config() : Config<cfg::Data>(new T()) { }
	Config(Config &config) : Config<cfg::Data>(config) { }

	operator T *()              { return  get(); }
	operator T &()              { return *get(); }
	operator const T *()  const { return  get(); }
	operator const T &()  const { return *get(); }
	T       &operator*()        { return *get(); }
	T       *operator->()       { return  get(); }
	T       *operator&()        { return  get(); }
	const T &operator*()  const { return *get(); }
	const T *operator->() const { return  get(); }
	const T *operator&()  const { return  get(); }

	Config &operator=(Config &config) {
		set(config);
		return *this;
	}

private:
	typedef Config<cfg::Data> parent;

	T       *get()       { return static_cast<T       *>(parent::get()); }
	const T *get() const { return static_cast<const T *>(parent::get()); }
};

}

#endif
