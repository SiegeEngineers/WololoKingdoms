#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <stdio.h>
#include "libwololokingdoms/platform.h"
#include "paths.h"

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

namespace fs = std::filesystem;

#ifdef _WIN32
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE,PBOOL);
#endif

fs::path extractHDPath(fs::path steamPath) {
	std::string line;
   fs::path HDPath;
	std::ifstream manifest(steamPath/"steamapps"/"appmanifest_221380.acf");
	while (std::getline(manifest,line)) {
		u_int i;
		if ((i = line.find("installdir")) != std::string::npos) {
			i = line.find("\"", i+11);
			int j = line.find("\"", i+1);
			line = line.substr(i+1,j-i-1);
			HDPath = steamPath/"steamapps"/"common"/line;
			break;
        }
	}
    manifest.close();
	return fs::path(HDPath);
}

#ifdef _WIN32
fs::path getSteamPath() {
	TCHAR temp[300];
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
   fs::path steamPath(wstrtostr(std::wstring(std::basic_string<TCHAR>(temp))));
	return steamPath;
}
#else
fs::path getSteamPath() {
  return "/home/goto-bus-stop/.local/share/Steam";
}
#endif

fs::path getHDPath(fs::path steamPath) {
	fs::path HDPath("../");
	std::string line;
	if(fs::exists(steamPath/"steamapps"/"appmanifest_221380.acf")) {
		HDPath = extractHDPath(steamPath);
	} else if (fs::exists(steamPath/"steamapps"/"libraryfolders.vdf")) {
		std::ifstream libraryFolders((steamPath/"steamapps"/"libraryfolders.vdf").c_str());
		while (std::getline(libraryFolders,line)) {
			u_int i;
			if ((i = line.find("\"1\"")) != std::string::npos || (i = line.find("\"2\"")) != std::string::npos || (i = line.find("\"3\"")) != std::string::npos) {
				i = line.find("\"", i+3);
				int j = line.find("\"", i+1);
				line = line.substr(i+1,j-i-1);
				steamPath = line;
				if(fs::exists(steamPath/"steamapps"/"appmanifest_221380.acf")) {
					HDPath = extractHDPath(steamPath);
					break;
				}
			}
		}
        libraryFolders.close();
	}

	if(!fs::exists(HDPath/"Launcher.exe")) {
		if(fs::exists("../../Launcher.exe")) {
			HDPath = fs::path("../../");
        } else if (fs::exists(steamPath/"steamapps"/"common"/"Age2HD")) {
            HDPath = fs::path(steamPath/"steamapps"/"common"/"Age2HD");
        } else { //Error Case
            HDPath = fs::path();
		}
	}
	return HDPath;
}

#ifdef _WIN32
fs::path getOutPath(fs::path HDPath) {

	TCHAR temp[300];
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
    std::string outPathString = wstrtostr(std::wstring(std::basic_string<TCHAR>(temp)));
    if(outPathString.at(outPathString.length()-1) != '\\')
        outPathString += "\\";
    fs::path outPath(outPathString);
    if(!fs::exists(outPath/"age2_x1")) {
		if(fs::exists(HDPath/"age2_x1")) {
			outPath = HDPath;
		} else {
			outPath = fs::path();
		}
	}
	return outPath;
}
#else
fs::path getOutPath(fs::path HDPath) {
  return fs::path("wololokingdoms_out");
}
#endif
