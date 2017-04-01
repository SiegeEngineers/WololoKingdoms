#ifndef CONVERSIONS_H
#define CONVERSIONS_H

DWORD ConvertUnicode2CP(const wchar_t *szText, std::string &resultString, UINT codePage = CP_ACP);
DWORD ConvertCP2Unicode(const char *szText, std::wstring &resultString, UINT codePage = CP_ACP);
std::wstring strtowstr( const std::string& as );
std::string wstrtostr( const std::wstring& as );
std::vector<std::string> split(const std::string &s, char delim);

#endif // CONVERSIONS_H
