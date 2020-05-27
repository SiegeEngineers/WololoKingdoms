#pragma once
#include <filesystem>

static std::filesystem::path extractHDPath(std::filesystem::path steamPath);
std::filesystem::path getExePath();
std::wstring readRegistryKey(std::wstring keyPath, std::wstring key);
std::filesystem::path getSteamPath();
std::filesystem::path getHDPath(std::filesystem::path steamPath);
std::filesystem::path getOutPath(std::filesystem::path HDPath);
