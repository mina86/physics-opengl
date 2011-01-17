/*
 * src/graph/randomise-graph.hpp
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
#ifndef H_RANDOMISE_GRAPH_HPP
#define H_RANDOMISE_GRAPH_HPP

#include "../ui/abstract-config.hpp"

namespace graph {

namespace rnd {

struct Data : public ui::cfg::Data {
	ui::cfg::Bool relative, normal, overwrite, reset;
	ui::cfg::Real r;

	virtual struct iterator items() const;

private:
	Data();

	friend struct ui::Config<Data>;

	Q_OBJECT
};

}

struct Scene;

struct GraphRandomiser {
	void randomise(Scene &scene);
	ui::cfg::Data &getConfigData() {
		return *config;
	}

private:
	ui::Config<rnd::Data> config;
};

}

#endif
