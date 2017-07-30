#ifndef PATHS_H
#define PATHS_H

#include <boost/filesystem.hpp>

std::string getSteamPath();
boost::filesystem::path getHDPath(std::string steamPath);
boost::filesystem::path getOutPath(boost::filesystem::path HDPath);

#endif // PATHS_H
