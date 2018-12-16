#pragma once
#include <map>
#include <filesystem>

#ifdef _WIN32

/**
 * Windows is already case insensitive.
 */
namespace cfs = std::filesystem;

#else

namespace fs = std::filesystem;

// Remember the casing of previously used paths.
static std::map<fs::path, fs::path> caseless_cache = {};

/**
 * Case-insensitively find a file path.
 */
static fs::path caseless(fs::path const& input) {
  std::string inputs = input.string();
  fs::path linput = tolower(inputs);
  if (caseless_cache.find(linput) != caseless_cache.end()) {
    return caseless_cache[linput];
  }

  auto parent_path = linput.parent_path();
  if (parent_path == linput) {
    return input;
  }

  auto parent = caseless(parent_path);
  // Failed already!
  if (!fs::is_directory(parent)) {
    return input;
  }

  auto filename = linput.filename().string();
  for (auto& entry : fs::directory_iterator(parent)) {
    auto str = entry.path().filename().string();
    if (tolower(str) == filename) {
      filename = str;
      caseless_cache[linput] = parent/filename;
      return parent/filename;
    }
  }
  return input;
}

/**
 * Case-insensitive wrappers around fs methods, for use on case sensitive file systems.
 */
namespace cfs {
  bool exists(fs::path p) {
    return fs::exists(caseless(p));
  }
  bool is_directory(fs::path p) {
    return fs::is_directory(caseless(p));
  }
  bool is_symlink(fs::path p) {
    return fs::is_symlink(caseless(p));
  }
  bool is_empty(fs::path p) {
    return fs::is_empty(caseless(p));
  }
  bool equivalent(fs::path a, fs::path b) {
    return fs::equivalent(caseless(a), caseless(b));
  }
  void remove(fs::path p) {
    fs::remove(caseless(p));
  }
  void remove_all(fs::path p) {
    fs::remove_all(caseless(p));
  }
  void create_directory(fs::path p) {
    fs::create_directory(caseless(p));
  }
  void create_directories(fs::path p) {
    fs::create_directories(caseless(p));
  }
  void copy_file(fs::path a, fs::path b) {
    fs::copy_file(caseless(a), caseless(b));
  }
  void copy_file(fs::path a, fs::path b, std::error_code& ec) {
    fs::copy_file(caseless(a), caseless(b), ec);
  }
  void copy_file(fs::path a, fs::path b, fs::copy_options options) {
    fs::copy_file(caseless(a), caseless(b), options);
  }
  void copy(fs::path a, fs::path b, fs::copy_options options) {
    fs::copy(caseless(a), caseless(b), options);
  }
  void rename(fs::path a, fs::path b) {
    fs::rename(caseless(a), caseless(b));
  }
  size_t file_size(fs::path p) {
    return fs::file_size(p);
  }
  fs::file_time_type last_write_time(fs::path p) {
    return fs::last_write_time(p);
  }
}
#endif
