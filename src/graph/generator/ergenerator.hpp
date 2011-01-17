/*
 * src/graph/generator/ergenerator.hpp
 * Copyright 2010 by Michal Nazarewicz <mina86@mina86.com>
 *               and Maciej Swietochowski
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
#ifndef ERGENERATOR_HPP
#define ERGENERATOR_HPP

#include "abstractgenerator.hpp"
#include "../../ui/abstract-config.hpp"

namespace graph {

namespace generator {

namespace er {

struct Data : public ui::cfg::Data {
	ui::cfg::Integer nodesCount;
	ui::cfg::Real p, radius;

	virtual struct iterator items() const;

private:
	Data();

	friend struct ui::Config<Data>;

	Q_OBJECT
};

}

struct ERGenerator : public AbstractGenerator {
	ERGenerator(QObject *parent = 0)
		: AbstractGenerator(parent) { }
	graph_ptr generate();
	ui::cfg::Data *getConfigData();

private:
	ui::Config<er::Data> config;

	Q_OBJECT
};

}

}

#endif
