#pragma once
#include <string>
#include <istream>
#include <sstream>

/**
 * Replace all occurrences of `from` with `to` in `str`.
 *
 * This mutates `str`.
 */
void replace_all(std::string& str, const std::string& from, const std::string& to);

/**
 * Convert a std::string to a std::wstring.
 */
std::wstring strtowstr(std::string narrow);

/**
 * Convert a std::wstring to a std::string.
 */
std::string wstrtostr(std::wstring wide);

/**
 * Convert a std::string to lowercase.
 */
std::string tolower(std::string line);

/**
 * Completely consume a readable stream and concatenate it into a std::string.
 */
std::string concat_stream(std::istream& stream);

/**
 * Convert a std::string from encoding `from` to encoding `to`.
 */
std::string iconvert(const std::string& input, const std::string& from, const std::string& to);
