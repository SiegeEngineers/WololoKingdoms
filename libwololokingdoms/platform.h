#ifndef _WK_PLATFORM_H
#define _WK_PLATFORM_H

#include <string>
#include <cstring>

/**
 * This contains platform specific functions; things that are different between Linux and Windows
 */

#define MKLINK_HARD 'H'
#define MKLINK_SOFT 's'
#define MKLINK_DIR 'd'
#define MKLINK_JUNCTION 'J'

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

static int ConvertUnicode2CP(const wchar_t *source, std::string &resultString)
{
  resultString.clear();
  if (wcslen(source) <= 0)
	return ERROR_SUCCESS;
  int iRes = WideCharToMultiByte(CP_ACP, 0, source, -1, NULL, 0, NULL, NULL);
  if (iRes <= 0)
	return GetLastError();
  char *szTemp = new char[iRes];

  iRes = WideCharToMultiByte(CP_ACP, 0, source, -1, szTemp, iRes, NULL, NULL);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

static int ConvertCP2Unicode(const char *source, std::wstring &resultString)
{
  resultString.clear();
  if (strlen(source) <= 0)
	return ERROR_SUCCESS;
  int iRes = MultiByteToWideChar(CP_ACP, 0, source, -1, NULL, 0);
  if (iRes <= 0)
	return GetLastError();
  wchar_t *szTemp = new wchar_t[iRes];

  iRes = MultiByteToWideChar(CP_ACP, 0, source, -1, szTemp, iRes);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

#else
/**
 * Linux
 */

#include <unistd.h>
#define ERROR_SUCCESS 0

#ifdef __GNUC__
#define ALLOW_UNUSED __attribute__ ((unused))
#else
#define ALLOW_UNUSED
#endif


static int ALLOW_UNUSED ConvertUnicode2CP(const wchar_t *source, std::string &resultString)
{
  resultString.clear();
  if (wcslen(source) <= 0)
	return ERROR_SUCCESS;
  return ERROR_SUCCESS;
}

static int ALLOW_UNUSED ConvertCP2Unicode(const char *source, std::wstring &resultString)
{
  resultString.clear();
  if (strlen(source) <= 0)
	return ERROR_SUCCESS;
  return ERROR_SUCCESS;
}

static void ALLOW_UNUSED mklink(ALLOW_UNUSED char type, std::string link, std::string dest)
{
  symlink(dest.c_str(), link.c_str());
}

#endif

#endif
