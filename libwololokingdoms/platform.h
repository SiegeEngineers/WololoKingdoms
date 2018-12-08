#ifndef _WK_PLATFORM_H
#define _WK_PLATFORM_H

#include <string>

#define MKLINK_HARD 'H'
#define MKLINK_DIR 'd'
#define MKLINK_JUNCTION 'J'

#ifdef _WIN32
/** 
 * Windows
 */
#include <windows.h>
#include <shellapi.h>

static void callExternalExe(std::wstring exe) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
    wchar_t cmdLineString[exe.length()+1];
    wcscpy(cmdLineString, exe.c_str());
    CreateProcess( nullptr, cmdLineString, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi );
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}

static void callWaitExe(std::wstring exe) {
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
  if (type == MKLINK_HARD) {
    CreateHardLinkA(link.c_str(), dest.c_str(), NULL);
  } else if (type == MKLINK_DIR) {
    CreateSymbolicLinkA(link.c_str(), dest.c_str(), SYMBOLIC_LINK_FLAG_DIRECTORY);
  } else if (type == MKLINK_JUNCTION) {
    std::wstringstream line;
    line << L"/C mklink /J " << strtowstr(link) << L" " << strtowstr(dest);
    callWaitExe(line.str().c_str());
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

static int ConvertUnicode2CP(const wchar_t *source, std::string &resultString)
{
  resultString.clear();
  if (wcslen(source) <= 0)
	return ERROR_SUCCESS;
  return ERROR_SUCCESS;
}

static int ConvertCP2Unicode(const char *source, std::wstring &resultString)
{
  resultString.clear();
  if (strlen(source) <= 0)
	return ERROR_SUCCESS;
  return ERROR_SUCCESS;
}

static void mklink(char type, std::string link, std::string dest) {
  symlink(dest.c_str(), link.c_str());
}

#endif

#endif
