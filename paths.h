#ifndef PATHS_H
#define PATHS_H

#include <filesystem>

namespace fs = std::filesystem;

std::string getSteamPath();
fs::path getHDPath(std::string steamPath);
fs::path getOutPath(fs::path HDPath);

#endif // PATHS_H
