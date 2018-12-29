#ifndef PATHS_H
#define PATHS_H

#include <filesystem>

namespace fs = std::filesystem;

fs::path getExePath();
fs::path getSteamPath();
fs::path getHDPath(fs::path steamPath);
fs::path getOutPath(fs::path HDPath);

#endif // PATHS_H
