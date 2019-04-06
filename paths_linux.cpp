#ifndef _WIN32
#include <iostream>
#include <fstream>
#include <string>
#include <fs.h>
#include <stdio.h>
#include "libwololokingdoms/platform.h"
#include "libwololokingdoms/string_helpers.h"
#include "paths.h"
#include <QProcess>
#include <pwd.h>
#include <iconv.h>
#include <errno.h>

fs::path getExePath() {
    return fs::read_symlink("/proc/self/exe");
}

static fs::path getHomeDirectory() {
  const char *homedir = getenv("HOME");
  if (homedir == NULL) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  return fs::path(homedir);
}

fs::path getSteamPath() {
  return getHomeDirectory()/".steam"/"steam";
}

static fs::path resolveWinePath(std::string winepath) {
  QProcess process;
  process.start("winepath", QStringList() << QString::fromStdString(winepath));
  process.waitForFinished();
  if (process.exitCode() != 0) {
    return fs::path();
  }
  QString result(process.readAllStandardOutput());
  return fs::canonical(result.trimmed().toStdString());
}

static std::string dump_wine_registry(std::string regkey) {
  fs::path tempFile("wk_tmp.reg");

  QProcess wine;
  wine.start("wine", QStringList()
      << QString("regedit")
      << QString("/E")
      << QString::fromStdString(tempFile.string())
      << QString::fromStdString(regkey));
  wine.waitForFinished();

  if (wine.exitCode() != 0) {
    return std::string();
  }

  std::ifstream stream(tempFile);
  auto result = concat_stream(stream);
  fs::remove(tempFile);
  return iconvert(result, "UTF16//IGNORE", "UTF8");
}

// On linux, we can still read the Wine registry
// by first dumping the Age of Empires key to a file
fs::path getOutPath([[maybe_unused]] fs::path hdPath) {
  std::stringstream registry (dump_wine_registry("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectPlay\\Applications\\Age of Empires II - The Conquerors Expansion\\"));
  std::string winepath;
  std::string prefix = "\"CurrentDirectory\"=";
  std::string line;
  while (std::getline(registry, line, '\n')) {
    if (line.substr(0, prefix.length()) == prefix) {
      winepath = line.substr(prefix.length());
      break;
    }
  }

  if (!winepath.empty()) {
    if (winepath[0] == '"')
      winepath = winepath.substr(1);
    // probably includes \r from \r\n
    if (winepath[winepath.length() - 1] == '\r')
      winepath = winepath.substr(0, winepath.length() - 1);
    if (winepath[winepath.length() - 1] == '"')
      winepath = winepath.substr(0, winepath.length() - 1);
    replace_all(winepath, "\\\\", "\\");
    return resolveWinePath(winepath);
  }

  return fs::path();
}
#endif
