/*
 * src/graph/data-loader.cpp
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
#include "../gl/abstract-scene.hpp"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <limits>

#include "../lib/lexer.hpp"

#include "scene.hpp"

namespace gl {

const char AbstractScene::extension[] = "grp";

/*
 * input	: { node | edge }
 * node		: "node" [ name ] { options } '\n'
 * edge		: "edge" [ name | INTEGER ] [ name | INTEGER ] '\n'
 * options	: position | color
 * position	: '@" NUMBER NUMBER NUMBER
 * color	: "color" NUMBER NUMBER NUMBER
 */

struct Node : public graph::Scene::Node {
	Node() {
		setPosition(std::numeric_limits<float>::quiet_NaN(),
		            std::numeric_limits<float>::quiet_NaN(),
		            std::numeric_limits<float>::quiet_NaN());
		setColor(1.0, 1.0, 1.0);
		color[3] = 0.0;
	}

	void setPosition(float x, float y, float z) {
		loadedPosition.set(x, y, z);
	}

	void setColor(float r, float g, float b) {
		color[0] = r;
		color[1] = g;
		color[2] = b;
	}
};

AbstractScene::ptr AbstractScene::load(std::istream &in)
	throw(std::ios_base::failure, lib::Lexer::error) {
	static lib::Lexer::Keywords keywords;

	enum {
		T_EOF     = lib::Lexer::T_EOF,
		T_INTEGER = lib::Lexer::T_INTEGER,
		T_REAL    = lib::Lexer::T_REAL,
		T_STRING  = lib::Lexer::T_STRING,
		T_NODE    = lib::Lexer::T_FIRST_KEYWOARD,
		T_EDGE,
		T_COLOR
	};

	if (keywords.empty()) {
		keywords.insert(std::pair<std::string, int>("node",    T_NODE));
		keywords.insert(std::pair<std::string, int>("edge",    T_EDGE));
		keywords.insert(std::pair<std::string, int>("color",   T_COLOR));
		keywords.insert(std::pair<std::string, int>("colour",  T_COLOR));
	}

	lib::Lexer lexer(in, &keywords);
	lexer.acceptInteger(false);
	lexer.acceptReal(false);

	typedef std::set< std::pair<unsigned, unsigned> > Edges;
	typedef std::map<std::string, unsigned> Names;
	typedef std::vector<Node> Nodes;

	Nodes nodes;
	Edges edges;
	Names names;

	unsigned token, prevToken, i, j;
	lib::Lexer::Location location;
	lib::Lexer::Value value;
	std::string name;
	float a, b, c;

#define NEXT()       (token = lexer.nextToken(value, location))
#define ACCEPT(_tok) do { if (NEXT() != (_tok)) goto error; } while (0)

#define NODE_NUMBER(n) do {												\
		switch (NEXT()) {												\
		case T_INTEGER:													\
			if (value.integer < 0 ||									\
			    (unsigned)value.integer >= nodes.size()) {				\
				throw lib::Lexer::error(location, "incorrect node number"); \
			}															\
			n = value.integer;											\
			break;														\
		case T_STRING: {												\
			Names::const_iterator it = names.find(value.string);		\
			if (it == names.end()) {									\
				throw lib::Lexer::error(location, lib::Lexer::error::msg(value.string, "invalid node name")); \
			}															\
			n = it->second;												\
		}																\
			break;														\
		default:														\
			goto error;													\
		}																\
	} while (0)

	for (NEXT(); token != T_EOF; ) {
		switch (token) {
		case T_NODE:
			nodes.push_back(Node());

			NEXT();
			if (token == T_STRING) {
				name = value.string;
				NEXT();
			} else {
				char buffer[32];
				sprintf(buffer, "#%u", (unsigned)(nodes.size() - 1));
				name.assign(buffer);
			}

			if (names.find(name) != names.end()) {
				throw lib::Lexer::error(location, lib::Lexer::error::msg(name, "name already used"));
			}

			names[name] = nodes.size() - 1;
			nodes.back().name = name;

			while (token == '@' || token == T_COLOR) {
				prevToken = token;

				lexer.acceptReal(true);
				ACCEPT(T_REAL); a = value.real;
				ACCEPT(T_REAL); b = value.real;
				ACCEPT(T_REAL); c = value.real;
				lexer.acceptReal(false);

				if (prevToken == '@') {
					nodes.back().setPosition(a, b, c);
				} else {
					nodes.back().setColor(a, b, c);
				}

				NEXT();
			}
			break;

		case T_EDGE:
			lexer.acceptInteger(true);

			NODE_NUMBER(i);
			NODE_NUMBER(j);

			lexer.acceptInteger(false);

			if (i == j) {
				throw lib::Lexer::error(location, "a loop edge");
			} else if (j < i) {
				std::swap(i, j);
			}

			edges.insert(std::pair<unsigned, unsigned>(i, j));
			NEXT();
			break;

		default:
		error:
			throw lib::Lexer::error(location, lib::Lexer::error::msg(lib::Lexer::tokenName(token, value), "unexpected token"));
		}
	}

	names.clear();


	if (nodes.empty() || edges.empty()) {
		throw lib::Lexer::error(location, "Graph has no nodes or no edges.");
	}


	graph::Scene *scene = new graph::Scene(nodes.size());
	AbstractScene::ptr ptr(scene);

	{
		Nodes::const_iterator in = nodes.begin(), end = nodes.end();
		graph::Scene::nodes_iterator out = scene->nodes_begin();

		for (; in != end; ++in, ++out) {
			if (isnan(in->loadedPosition.x())) {
				(*out).first.zero();
			} else {
				(*out).first = in->loadedPosition;
			}
			(*out).second = *in;
		}
	}

	{
		Edges::const_iterator in = edges.begin(), end = edges.end();

		for (; in != end; ++in) {
			scene->e(in->first, in->second, true);
		}
	}


	return ptr;
}

}

