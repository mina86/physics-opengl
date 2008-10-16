#ifndef H_DATA_LOADER_HPP
#define H_DATA_LOADER_HPP

#include <fstream>
#include <stdexcept>
#include <vector>

namespace mn {

namespace solar {

struct Sphere;

Sphere *loadData(const std::string &filename);

}

}

#endif
