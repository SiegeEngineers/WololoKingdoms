#include <iostream>
#include <set>
#include <map>
#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#include <windows.h>

DWORD ConvertUnicode2CP(const wchar_t *szText, std::string &resultString, UINT codePage)
{
  resultString.clear();
  if (wcslen(szText) <= 0)
	return ERROR_SUCCESS;
  int iRes = WideCharToMultiByte(codePage, 0, szText, -1, NULL, 0, NULL, NULL);
  if (iRes <= 0)
	return GetLastError();
  char *szTemp = new char[iRes];

  iRes = WideCharToMultiByte(codePage, 0, szText, -1, szTemp, iRes, NULL, NULL);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

DWORD ConvertCP2Unicode(const char *szText, std::wstring &resultString, UINT codePage)
{
  resultString.clear();
  if (strlen(szText) <= 0)
	return ERROR_SUCCESS;
  int iRes = MultiByteToWideChar(codePage, 0, szText, -1, NULL, 0);
  if (iRes <= 0)
	return GetLastError();
  wchar_t *szTemp = new wchar_t[iRes];

  iRes = MultiByteToWideChar(codePage, 0, szText, -1, szTemp, iRes);
  if (iRes <= 0)
  {
	delete [] szTemp;
	return GetLastError();
  }

  resultString = szTemp;
  delete [] szTemp;
  return ERROR_SUCCESS;
}

std::wstring strtowstr( const std::string& as )
{
			// deal with trivial case of empty string
	if( as.empty() )    return std::wstring();

			// determine required length of new string
	size_t reqLength = ::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0 );

			// construct new string of required length
	std::wstring ret( reqLength, L'\0' );

			// convert old string to new string
	::MultiByteToWideChar( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length() );

			// return new string ( compiler should optimize this away )
	return ret;
}

std::string wstrtostr( const std::wstring& as )
{
			// deal with trivial case of empty string
	if( as.empty() )    return std::string();

			// determine required length of new string
	size_t reqLength = ::WideCharToMultiByte( CP_UTF8, 0, as.c_str(), (int)as.length(), 0, 0, 0, 0 );

			// construct new string of required length
	std::string ret( reqLength, L'\0' );

			// convert old string to new string
	::WideCharToMultiByte( CP_UTF8, 0, as.c_str(), (int)as.length(), &ret[0], (int)ret.length(), 0, 0 );

			// return new string ( compiler should optimize this away )
	return ret;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	std::back_insert_iterator<std::vector<std::string>> result = std::back_inserter(elems);
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
	return elems;
}
