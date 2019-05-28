#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <fs.h>
#include <stdio.h>
#include "libwololokingdoms/platform.h"
#include "libwololokingdoms/string_helpers.h"
#include "paths.h"

#ifdef _WIN32
#include <windows.h>
#endif

static fs::path extractHDPath(fs::path steamPath) {
  std::string line;
  fs::path hdPath;
  std::ifstream manifest(steamPath/"steamapps"/"appmanifest_221380.acf");
  while (std::getline(manifest,line)) {
    size_t i;
    if ((i = line.find("installdir")) != std::string::npos) {
      i = line.find("\"", i+11);
      int j = line.find("\"", i+1);
      line = line.substr(i+1,j-i-1);
      hdPath = steamPath/"steamapps"/"common"/line;
      break;
    }
  }
  manifest.close();
  return fs::path(hdPath);
}

fs::path getHDPath(fs::path steamPath) {
  fs::path hdPath("../");
  std::string line;
  if(fs::exists(steamPath/"steamapps"/"appmanifest_221380.acf")) {
    hdPath = extractHDPath(steamPath);
  } else if (fs::exists(steamPath/"steamapps"/"libraryfolders.vdf")) {
    std::ifstream libraryFolders(steamPath/"steamapps"/"libraryfolders.vdf");
    while (std::getline(libraryFolders,line)) {
      size_t i;
      if ((i = line.find("\"1\"")) != std::string::npos || (i = line.find("\"2\"")) != std::string::npos || (i = line.find("\"3\"")) != std::string::npos) {
        i = line.find("\"", i+3);
        int j = line.find("\"", i+1);
        line = line.substr(i+1,j-i-1);
        steamPath = line;
        if(fs::exists(steamPath/"steamapps"/"appmanifest_221380.acf")) {
          hdPath = extractHDPath(steamPath);
          break;
        }
      }
    }
    libraryFolders.close();
  }

  if(!fs::exists(hdPath/"Launcher.exe")) {
    if(fs::exists("../../Launcher.exe")) {
      hdPath = fs::path("../../");
    } else if (fs::exists(steamPath/"steamapps"/"common"/"Age2HD")) {
      hdPath = fs::path(steamPath/"steamapps"/"common"/"Age2HD");
    } else { //Error Case
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

fs::path getSteamPath() {
  wchar_t temp[300];
  unsigned long size = sizeof(temp);
  HKEY hKey;

  BOOL w64;
  IsWow64Process(GetCurrentProcess(), &w64);
  if(w64)
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\Valve\\Steam", 0, KEY_READ, &hKey);
  else
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Valve\\Steam", 0, KEY_READ, &hKey);
  RegQueryValueEx(hKey, L"InstallPath", NULL, NULL, reinterpret_cast<LPBYTE>(temp), &size);
  RegCloseKey(hKey);
  auto steamPath = std::wstring(temp);
  return steamPath;
}

fs::path getOutPath(fs::path hdPath) {
  wchar_t temp[300];
  unsigned long size = sizeof(temp);
  HKEY hKey;

  BOOL w64;
  IsWow64Process(GetCurrentProcess(), &w64);
  if(w64)
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\WOW6432Node\\Microsoft\\DirectPlay\\Applications\\Age of Empires II - The Conquerors Expansion", 0, KEY_READ, &hKey);
  else
    RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\DirectPlay\\Applications\\Age of Empires II - The Conquerors Expansion", 0, KEY_READ, &hKey);
  RegQueryValueEx(hKey, L"CurrentDirectory", NULL, NULL, reinterpret_cast<LPBYTE>(temp), &size);
  RegCloseKey(hKey);
  std::string outPathString = wstrtostr(std::wstring(temp));
  if(outPathString.at(outPathString.length()-1) != '\\')
    outPathString += "\\";
  fs::path outPath(outPathString);
  if(!fs::exists(outPath/"age2_x1")) {
    if(fs::exists(hdPath/"age2_x1")) {
      outPath = hdPath;
    } else {
      outPath = fs::path();
    }
  }
  return outPath;
}
#endif
