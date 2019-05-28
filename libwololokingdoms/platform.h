#pragma once
#include <string>

/**
 * This contains platform specific functions; things that are different between Linux and Windows
 */

#define MKLINK_SOFT 's'
#define MKLINK_DIR 'd'

#ifdef _WIN32
/**
 * Windows
 */
#include <windows.h>
#include <shellapi.h>
#include <sstream>

static void runCmd(std::wstring exe) {
    SHELLEXECUTEINFO ShExecInfo = {0};
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = nullptr;
    ShExecInfo.lpVerb = nullptr;
    ShExecInfo.lpFile = L"cmd.exe";
    ShExecInfo.lpParameters = exe.c_str();
    ShExecInfo.lpDirectory = nullptr;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = nullptr;
    ShellExecuteEx(&ShExecInfo);
    WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
}

static void mklink(char type, std::string link, std::string dest) {
  std::wstring wlink;
  std::wstring wdest;
  for (auto c : link) {
    wchar_t w = c;
    wlink.push_back(w);
  }
  for (auto c : dest) {
    wchar_t w = c;
    wdest.push_back(w);
  }

  if (type == MKLINK_SOFT) {
    std::wstringstream line;
    line << L"/C mklink " << wlink << L" " << wdest;
    runCmd(line.str().c_str());
  } else if (type == MKLINK_DIR) {
#ifdef CreateSymbolicLink
    CreateSymbolicLink(wlink.c_str(), wdest.c_str(), SYMBOLIC_LINK_FLAG_DIRECTORY);
#else
    std::wstringstream line;
    line << L"/C mklink /d " << wlink << L" " << wdest;
    runCmd(line.str().c_str());
#endif
  }
}

#else
/**
 * Linux
 */

#include <unistd.h>

static void mklink [[maybe_unused]] ([[maybe_unused]] char type, std::string link, std::string dest)
{
  symlink(dest.c_str(), link.c_str());
}

#endif
