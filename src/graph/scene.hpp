/*
 * src/graph/scene.hpp
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
#ifndef H_SCENE_HPP
#define H_SCENE_HPP

#include <utility>

#include "../gl/abstract-scene.hpp"

#include "graph.hpp"

namespace graph {

struct Scene : public gl::AbstractScene, public Graph {
	struct Node {
		std::string name;
		float color[4];
		/* This is the position that has been loaded from file (or
		 * generated or whatever).  If isnan(loadedPosetion.x()) it
		 * means that no position was spocified in the loaded file. */
		gl::Vector<float> loadedPosition;
	};

	typedef std::pair<Graph::node_type &, Node &> node_type;
	typedef std::pair<const Graph::node_type &, const Node &> const_node_type;

private:
	void commonInit();

public:
	explicit Scene(unsigned theN) throw(std::invalid_argument, std::bad_alloc);
	explicit Scene(const Graph &g);
	~Scene();

	/* No checking of v is done.  Beware! */
	node_type n(unsigned v) {
		return node_type(Graph::n(v), nodes_ext_vec[v]);
	}
	const_node_type n(unsigned v) const {
		return const_node_type(Graph::n(v), nodes_ext_vec[v]);
	}

	struct nodes_iterator
		: public std::iterator<std::bidirectional_iterator_tag, node_type> {

		nodes_iterator(struct Scene &theScene, unsigned n)
			: scene(&theScene), v(n) { }
		nodes_iterator() : scene(NULL), v(0) { }

		nodes_iterator &operator++() { ++v; return *this; }
		nodes_iterator  operator++(int) {
			return nodes_iterator(*scene, v++);
		}
		nodes_iterator &operator--() { --v; return *this; }
		nodes_iterator  operator--(int) {
			return nodes_iterator(*scene, v--);
		}

		// node_type *operator->() { return &(scene->n(v)); }
		node_type  operator*()  { return   scene->n(v) ; }

		bool operator==(const nodes_iterator &it) const { return v == it.v; }
		bool operator!=(const nodes_iterator &it) const { return v != it.v; }

	private:
		Scene *scene;
		unsigned v;
	};

	nodes_iterator nodes_begin() { return nodes_iterator(*this, 0); }
	nodes_iterator nodes_end()   { return nodes_iterator(*this, nodes()); }

	void initializeGL(const gl::Widget &gl);
	void drawScene(const gl::Widget &gl);
	void updateState(unsigned ticks, float dt);
	void save(std::ostream &out) throw(std::ios_base::failure);

private:
	Scene();

	void saveNodes(std::ostream &out) throw(std::ios_base::failure);
	void saveEdges(std::ostream &out) throw(std::ios_base::failure);

	Node *nodes_ext_vec;
};

}

#endif
