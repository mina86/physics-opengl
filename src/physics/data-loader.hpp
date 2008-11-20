#ifndef H_DATA_LOADER_HPP
#define H_DATA_LOADER_HPP

#include <fstream>
#include <stdexcept>
#include <vector>

namespace mn {

namespace physics {

struct Object;

Object *loadData(const char *filename);

}

}

#endif
