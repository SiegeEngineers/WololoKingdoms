#pragma once
#include <string>
#include <cstring>
#include <iconv.h>

/**
 * This contains platform specific functions; things that are different between Linux and Windows
 */

#define MKLINK_HARD 'H'
#define MKLINK_SOFT 's'
#define MKLINK_DIR 'd'
#define MKLINK_JUNCTION 'J'

#ifdef __GNUC__
#define ALLOW_UNUSED __attribute__ ((unused))
#else
#define ALLOW_UNUSED
#endif

static std::string iconvert (const std::string& input, const std::string& from, const std::string& to) {
  char* in_str = const_cast<char*>(input.c_str());
  auto in_size = input.length();
  size_t out_size = in_size * 2;
  char* result = new char[out_size];
  char* out = result; // separate value because iconv advances the pointer

  iconv_t convert = iconv_open(to.c_str(), from.c_str());
  if (convert == (iconv_t) -1) {
    return "";
  }
  if (iconv(convert, &in_str, &in_size, &out, &out_size) == (size_t) -1) {
    return "";
  }

  iconv_close(convert);

  return result;
}

static std::string ALLOW_UNUSED ConvertUnicode2CP(const std::string& source)
{
  return iconvert(source, "UTF8", "WINDOWS-1252");
}

static std::string ALLOW_UNUSED ConvertCP2Unicode(const std::string& source)
{
  return iconvert(source, "WINDOWS-1252", "UTF8");
}

#ifdef _WIN32
/**
 * Windows
 */
#include <windows.h>
#include <shellapi.h>

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

  if (type == MKLINK_HARD) {
    CreateHardLink(wlink.c_str(), wdest.c_str(), NULL);
  } else if (type == MKLINK_SOFT) {
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
  } else if (type == MKLINK_JUNCTION) {
    std::wstringstream line;
    line << L"/C mklink /J " << wlink << L" " << wdest;
    runCmd(line.str().c_str());
  }
}

#else
/**
 * Linux
 */

#include <unistd.h>

static void ALLOW_UNUSED mklink(ALLOW_UNUSED char type, std::string link, std::string dest)
{
  symlink(dest.c_str(), link.c_str());
}

#endif
