#ifndef PATHS_H
#define PATHS_H

#include <boost/filesystem.hpp>

boost::filesystem::path getHDPath();
boost::filesystem::path getOutPath(boost::filesystem::path HDPath);

#endif // PATHS_H
