#include <string>
#include <istream>
#include <algorithm>
#include <iconv.h>
#include <cctype> // std::tolower
#include "string_helpers.h"

void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

std::wstring strtowstr(std::string narrow) {
    std::wstring wide;
    for (auto c : narrow) {
        wchar_t w = c;
        wide.push_back(w);
    }
    return wide;
}

std::string wstrtostr(std::wstring wide) {
    std::string narrow;
    for (auto c : wide) {
        char n = c;
        narrow.push_back(n);
    }
    return narrow;
}

std::string tolower(std::string line) {
	std::transform(line.begin(), line.end(), line.begin(), static_cast<int(*)(int)>(std::tolower));
	return line;
}

std::string concat_stream(std::istream& stream) {
    std::stringstream strstr;
    strstr << stream.rdbuf();
    return strstr.str();
}

std::string iconvert (const std::string& input, const std::string& from, const std::string& to) {
#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
  const char* in_str = input.c_str();
#else
  char* in_str = const_cast<char*>(input.c_str());
#endif
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
  *out = '\0';

  iconv_close(convert);

  return result;
}
