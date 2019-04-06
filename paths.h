#pragma once
#include <fs.h>

fs::path getExePath();
fs::path getSteamPath();
fs::path getHDPath(fs::path steamPath);
fs::path getOutPath(fs::path HDPath);
