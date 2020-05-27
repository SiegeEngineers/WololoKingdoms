#pragma once
#include <filesystem>

#ifdef _WIN32

/**
 * Windows is already case insensitive.
 */
namespace cfs = std::filesystem;

#else
#include "./string_helpers.h"
#include <map>

// Remember the casing of previously used paths.
static std::map<std::filesystem::path, std::filesystem::path>
    caseless_directories = {};
static std::map<std::filesystem::path,
                std::map<std::filesystem::path, std::filesystem::path>>
    caseless_files = {};

/**
 * Case-insensitively find a file path.
 */
static const std::filesystem::path&
caseless(std::filesystem::path const& input) {
  std::string inputs = input.string();
  std::filesystem::path linput = tolower(inputs);
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
    if (!std::filesystem::is_directory(parent)) {
      return input;
    }

    caseless_directories[parent_path] = parent;
    auto& cached_dir = caseless_files[parent];
    for (auto& entry : std::filesystem::directory_iterator(parent)) {
      auto basename = entry.path().filename();
      cached_dir[tolower(basename.string())] = parent / basename;
    }
  }

  auto& directory = caseless_files[parent];
  if (directory.find(filename) == directory.end()) {
    // Creates a new file, use the casing used in the std::filesystem call
    directory[filename] = parent / input.filename();
  }

  return directory[filename];
}

/**
 * Case-insensitive wrappers around std::filesystem methods, for use on case
 * sensitive file systems.
 */
namespace cfs {
namespace fs = std::filesystem;
using path = fs::path;
using copy_options = fs::copy_options;
static path resolve [[maybe_unused]] (const path& p) { return caseless(p); }
static bool exists [[maybe_unused]] (const path& p) {
  return fs::exists(caseless(p));
}
static bool is_directory [[maybe_unused]] (const path& p) {
  return fs::is_directory(caseless(p));
}
static bool is_symlink [[maybe_unused]] (const path& p) {
  return fs::is_symlink(caseless(p));
}
static bool is_empty [[maybe_unused]] (const path& p) {
  return fs::is_empty(caseless(p));
}
static bool equivalent [[maybe_unused]] (const path& a, const path& b) {
  return fs::equivalent(caseless(a), caseless(b));
}
static void remove [[maybe_unused]] (const path& p) { fs::remove(caseless(p)); }
static void remove_all [[maybe_unused]] (const path& p) {
  fs::remove_all(caseless(p));
}
static void create_directory [[maybe_unused]] (const path& p) {
  fs::create_directory(caseless(p));
}
static void create_directories [[maybe_unused]] (const path& p) {
  fs::create_directories(caseless(p));
}
static void copy_file [[maybe_unused]] (const path& a, const path& b) {
  fs::copy_file(caseless(a), caseless(b));
}
static void copy_file
    [[maybe_unused]] (const path& a, const path& b, std::error_code& ec) {
  fs::copy_file(caseless(a), caseless(b), ec);
}
static void copy_file
    [[maybe_unused]] (const path& a, const path& b, copy_options options) {
  fs::copy_file(caseless(a), caseless(b), options);
}
static void copy
    [[maybe_unused]] (const path& a, const path& b, copy_options options) {
  fs::copy(caseless(a), caseless(b), options);
}
static void rename [[maybe_unused]] (const path& a, const path& b) {
  fs::rename(caseless(a), caseless(b));
}
static size_t file_size [[maybe_unused]] (const path& p) {
  return fs::file_size(caseless(p));
}
static fs::file_time_type last_write_time [[maybe_unused]] (const path& p) {
  return fs::last_write_time(caseless(p));
}
} // namespace cfs
#endif
