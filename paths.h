#pragma once
#include <fs.h>

static fs::path extractHDPath(fs::path steamPath);
fs::path getExePath();
std::wstring readRegistryKey(std::wstring keyPath, std::wstring key);
fs::path getSteamPath();
fs::path getHDPath(fs::path steamPath);
fs::path getOutPath(fs::path HDPath);
