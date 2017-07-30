#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/filesystem.hpp>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "paths.h"
#include "conversions.h"

namespace fs = boost::filesystem;

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE,PBOOL);

fs::path extractHDPath(std::string steamPath) {
	std::string line;
	std::string HDPath;
	std::ifstream manifest((steamPath+"/steamapps/appmanifest_221380.acf").c_str());
	while (std::getline(manifest,line)) {
		u_int i;
		if ((i = line.find("installdir")) != std::string::npos) {
			i = line.find("\"", i+11);
			int j = line.find("\"", i+1);
			line = line.substr(i+1,j-i-1);
			HDPath = steamPath+"/steamapps/common/"+line+"/";
			break;
		}
	}
	return fs::path(HDPath);
}

std::string getSteamPath() {
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
	std::string steamPath(wstrtostr(std::wstring(std::basic_string<TCHAR>(temp))));
	return steamPath;
}

fs::path getHDPath(std::string steamPath) {
	fs::path HDPath("../");
	std::string line;
	if(boost::filesystem::exists(steamPath+"/steamapps/appmanifest_221380.acf")) {
		HDPath = extractHDPath(steamPath);
	} else if (boost::filesystem::exists(steamPath+"/steamapps/libraryfolders.vdf")) {
		std::ifstream libraryFolders((steamPath+"/steamapps/libraryfolders.vdf").c_str());
		while (std::getline(libraryFolders,line)) {
			u_int i;
			if ((i = line.find("\"1\"")) != std::string::npos || (i = line.find("\"2\"")) != std::string::npos || (i = line.find("\"3\"")) != std::string::npos) {
				i = line.find("\"", i+3);
				int j = line.find("\"", i+1);
				line = line.substr(i+1,j-i-1);
				steamPath = line;
				if(boost::filesystem::exists(steamPath+"/steamapps/appmanifest_221380.acf")) {
					HDPath = extractHDPath(steamPath);
					break;
				}
			}
		}
	}

	if(!boost::filesystem::exists(HDPath/"/Launcher.exe")) {
		if(boost::filesystem::exists("../../Launcher.exe")) {
			HDPath = fs::path("../../");
		} else {
			return "";
		}
	}
	return HDPath;
}

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
	fs::path outPath(wstrtostr(std::wstring(std::basic_string<TCHAR>(temp))));
	bool aocFound = false;
	if(boost::filesystem::exists(outPath))
		aocFound = true;
	if(!aocFound) {
		if(boost::filesystem::exists(HDPath / "age2_x1")) {
			outPath = HDPath;
			aocFound = true;
		} else {
			outPath = HDPath/"WololoKingdoms/out/";
		}
	}
	return outPath;
}
