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
static std::map<fs::path, fs::path> caseless_directories = {};
static std::map<fs::path, std::map<fs::path, fs::path>> caseless_files = {};

/**
 * Case-insensitively find a file path.
 */
static const fs::path& caseless(fs::path const& input) {
  std::string inputs = input.string();
  fs::path linput = tolower(inputs);
  if (caseless_directories.find(linput) != caseless_directories.end()) {
    return caseless_directories[linput];
  }

  auto parent_path = linput.parent_path();
  // If the parent is the same as the current path
  // we can't go up more levels; just return and hope
  // for the best
  if (parent_path == linput || parent_path.empty()) {
    return input;
  }

  auto& parent = caseless(parent_path);

  auto filename = linput.filename();
  if (caseless_files.find(parent) == caseless_files.end()) {
    // Invalid path, trying to use a nonexistent directory
    if (!fs::is_directory(parent)) {
      return input;
    }

    caseless_directories[parent_path] = parent;
    auto& cached_dir = caseless_files[parent];
    for (auto& entry : fs::directory_iterator(parent)) {
      auto basename = entry.path().filename();
      cached_dir[tolower(basename.string())] = parent/basename;
    }
  }

  auto& directory = caseless_files[parent];
  if (directory.find(filename) == directory.end()) {
    // Creates a new file, use the casing used in the fs call
    directory[filename] = parent/input.filename();
  }

  return directory[filename];
}

/**
 * Case-insensitive wrappers around fs methods, for use on case sensitive file systems.
 */
namespace cfs {
  fs::path resolve(const fs::path& p) {
    return caseless(p);
  }
  bool exists(const fs::path& p) {
    return fs::exists(caseless(p));
  }
  bool is_directory(const fs::path& p) {
    return fs::is_directory(caseless(p));
  }
  bool is_symlink(const fs::path& p) {
    return fs::is_symlink(caseless(p));
  }
  bool is_empty(const fs::path& p) {
    return fs::is_empty(caseless(p));
  }
  bool equivalent(const fs::path& a, const fs::path& b) {
    return fs::equivalent(caseless(a), caseless(b));
  }
  void remove(const fs::path& p) {
    fs::remove(caseless(p));
  }
  void remove_all(const fs::path& p) {
    fs::remove_all(caseless(p));
  }
  void create_directory(const fs::path& p) {
    fs::create_directory(caseless(p));
  }
  void create_directories(const fs::path& p) {
    fs::create_directories(caseless(p));
  }
  void copy_file(const fs::path& a, const fs::path& b) {
    fs::copy_file(caseless(a), caseless(b));
  }
  void copy_file(const fs::path& a, const fs::path& b, std::error_code& ec) {
    fs::copy_file(caseless(a), caseless(b), ec);
  }
  void copy_file(const fs::path& a, const fs::path& b, fs::copy_options options) {
    fs::copy_file(caseless(a), caseless(b), options);
  }
  void copy(const fs::path& a, const fs::path& b, fs::copy_options options) {
    fs::copy(caseless(a), caseless(b), options);
  }
  void rename(const fs::path& a, const fs::path& b) {
    fs::rename(caseless(a), caseless(b));
  }
  size_t file_size(const fs::path& p) {
    return fs::file_size(caseless(p));
  }
  fs::file_time_type last_write_time(const fs::path& p) {
    return fs::last_write_time(caseless(p));
  }
}
#endif
