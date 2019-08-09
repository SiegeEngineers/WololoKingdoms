#include "paths.h"
#include "libwololokingdoms/platform.h"
#include "libwololokingdoms/string_helpers.h"
#include <fs.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <steam/steam_api.h>

#ifdef _WIN32
#include <Windows.h>
#endif

static fs::path extractHDPath(fs::path steamPath) {
  std::string line;
  fs::path hdPath;
  std::ifstream manifest(steamPath / "steamapps" / "appmanifest_221380.acf");
  while (std::getline(manifest, line)) {
    size_t i;
    if ((i = line.find("installdir")) != std::string::npos) {
      i = line.find("\"", i + 11);
      int j = line.find("\"", i + 1);
      line = line.substr(i + 1, j - i - 1);
      hdPath = steamPath / "steamapps" / "common" / line;
      break;
    }
  }
  manifest.close();
  return fs::path(hdPath);
}

fs::path getHDPath(fs::path steamPath) {
  fs::path hdPath("../");
  std::string line;
  if (fs::exists(steamPath / "steamapps" / "appmanifest_221380.acf")) {
    hdPath = extractHDPath(steamPath);
  } else if (fs::exists(steamPath / "steamapps" / "libraryfolders.vdf")) {
    std::ifstream libraryFolders(steamPath / "steamapps" /
                                 "libraryfolders.vdf");
    while (std::getline(libraryFolders, line)) {
      size_t i;
      if ((i = line.find("\"1\"")) != std::string::npos ||
          (i = line.find("\"2\"")) != std::string::npos ||
          (i = line.find("\"3\"")) != std::string::npos) {
        i = line.find("\"", i + 3);
        int j = line.find("\"", i + 1);
        line = line.substr(i + 1, j - i - 1);
        steamPath = line;
        if (fs::exists(steamPath / "steamapps" / "appmanifest_221380.acf")) {
          hdPath = extractHDPath(steamPath);
          break;
        }
      }
    }
    libraryFolders.close();
  }

  if (!fs::exists(hdPath / "Launcher.exe")) {
    if (fs::exists("../../Launcher.exe")) {
      hdPath = fs::path("../../");
    } else if (fs::exists(steamPath / "steamapps" / "common" / "Age2HD")) {
      hdPath = fs::path(steamPath / "steamapps" / "common" / "Age2HD");
    } else { // Error Case
      hdPath = fs::path();
    }
  }
  return hdPath;
}

#ifdef _WIN32
fs::path getExePath() {
  wchar_t exePath[MAX_PATH];
  auto pathLength = GetModuleFileName(nullptr, exePath, MAX_PATH);
  if (pathLength == 0) {
    return fs::path();
  }
  auto str = std::wstring(exePath);
  return fs::path(str);
}

std::wstring readRegistryKey(std::wstring keyPath, std::wstring key) {
    wchar_t temp[300];
    unsigned long size = sizeof(temp);
    HKEY hKey;

    BOOL w64;
    IsWow64Process(GetCurrentProcess(), &w64);
    LONG result = 0;
    for(int i = 0; i < 2; i++) {
        if (w64)
          result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (L"Software\\WOW6432Node\\"+keyPath).c_str(), 0,
                       KEY_READ, &hKey);
        else
          result = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (L"Software\\"+keyPath).c_str(), 0, KEY_READ,
                       &hKey);
        if (result == ERROR_SUCCESS)
            break;
        w64 = !w64; //If the key read wasn't successful, maybe the 32/64-bit check was erroneous, we'll check the other key
    }
    if (result != ERROR_SUCCESS)
        return L"";
    RegQueryValueEx(hKey, key.c_str(), nullptr, nullptr,
                    reinterpret_cast<LPBYTE>(temp), &size);
    RegCloseKey(hKey);
    return std::wstring(temp);
}

fs::path getSteamPath() {
  return readRegistryKey(L"Valve\\Steam", L"InstallPath");
}

fs::path getOutPath(fs::path hdPath) {
  std::string outPathString = wstrtostr(
              readRegistryKey(L"Microsoft\\DirectPlay\\Applications\\Age of "
                                L"Empires II - The Conquerors Expansion", L"CurrentDirectory"));
  if (outPathString.at(outPathString.length() - 1) != '\\')
    outPathString += "\\";
  fs::path outPath(outPathString);
  if (!fs::exists(outPath / "age2_x1")) {
    if (fs::exists(outPath.parent_path().parent_path() / "age2_x1")) { //If currentDirectory points one level too deep, had this happen with a faulty aoe2tools installation
      outPath = outPath.parent_path().parent_path(); //ParentPath needs to be called twice because the first one only removes the trailing /
    } else if (fs::exists(hdPath / "age2_x1")) {
      outPath = hdPath;
    } else {
      outPath = fs::path();
    }
  }
  return outPath;
}
#endif
