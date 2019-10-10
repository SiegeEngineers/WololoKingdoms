#pragma once
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "platform.h"
#include "wksettings.h"
#include <fs.h>
#include <map>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <vector>

/**
 * WK Conversion event listener, used to update the UI and to implement
 * platform/app-specific things like UserPatch installation.
 */
class WKConvertListener {
  int m_cachedProgress = 0;

public:
  virtual ~WKConvertListener() = default;

  /**
   * Called when conversion has finished.
   */
  virtual void finished() {}

  /**
   * Log a message.
   */
  virtual void log([[maybe_unused]] std::string logMessage) {}

  /**
   * Set the information line, usually describing what the converter is
   * currently working on.
   */
  virtual void setInfo([[maybe_unused]] std::string info) {}

  /**
   * Report an error.
   */
  virtual void error([[maybe_unused]] std::exception const& err,
                     bool showDialog = false) {}

  /**
   * Report an error (message only).
   */
  virtual void error([[maybe_unused]] std::string message,
                     bool showDialog = false) {
    error(std::runtime_error(message), showDialog);
  }

  /**
   * Create a dialog (this can usually be a noop if you're not building a UI)
   */
  virtual void createDialog([[maybe_unused]] std::string info) {}

  /**
   * Create a dialog (this can usually be a noop if you're not building a UI)
   */
  virtual void createDialog([[maybe_unused]] std::string info,
                            [[maybe_unused]] std::string title) {}

  /**
   * Create a dialog (this can usually be a noop if you're not building a UI)
   */
  virtual void createDialog([[maybe_unused]] std::string info,
                            [[maybe_unused]] std::string toReplace,
                            [[maybe_unused]] std::string replaceWith) {}

  /**
   * Set the current progress (0-100).
   */
  virtual void setProgress([[maybe_unused]] int i) {}

  /**
   * Increase the current progress.
   */
  virtual void increaseProgress([[maybe_unused]] int i) {
    m_cachedProgress += i;
    setProgress(m_cachedProgress);
  }

  /**
   * Install UserPatch.
   * userPatchExe is the path to the UserPatch SetupAoC.exe file.
   * cliFlags are strings that should be passed to the installer as command-line
   * flags
   */
  virtual void
  installUserPatch([[maybe_unused]] fs::path userPatchExe,
                   [[maybe_unused]] std::vector<std::string> cliFlags) {}
};

class WKConverter {
  WKSettings settings;
  WKConvertListener* listener;

public:
  WKConverter(WKSettings& settings, WKConvertListener* const listener)
      : settings(settings), listener(listener) {
    if (settings.resourceDir != fs::path()) {
      resourceDir = settings.resourceDir;
    }
  }

  int run();
  int retryInstall();

private:
  std::set<char> civLetters;
  std::set<int> aocSlpFiles;
  std::map<int, fs::path> slpFiles;
  std::map<int, fs::path> wavFiles;
  std::map<std::string, fs::path> newTerrainFiles;
  std::vector<std::pair<int, std::string>> rmsCodeStrings;
  bool secondAttempt = false;

  fs::path nfzUpOutPath;
  fs::path nfzVooblyOutPath;
  fs::path vooblyDir;
  fs::path upDir;
  fs::path installDir;
  std::string baseModName = "WololoKingdoms";
  fs::path resourceDir = "resources";

  enum TerrainType {
    None,
    WaterTerrain,
    FixedTerrain,
    LandTerrain,
    ForestTerrain,
    UnbuildableTerrain
  };

  struct MapConvertData {
    std::string slp_name;
    std::vector<std::string> const_names;
    std::string replaced_name;
    int old_terrain_id;
    int new_terrain_id;
    TerrainType terrain_type;
  };

  void copyHDMaps(const fs::path& inputDir, const fs::path& outputDir,
                  bool replace = false);
  static bool usesMultipleWaterTerrains(const std::string& map,
                                        std::map<int, bool>& terrainsUsed);
  static bool isTerrainUsed(int terrain, std::map<int, bool>& terrainsUsed,
                            const std::string& map,
                            const std::map<int, std::regex>& patterns);
  static void upgradeTrees(int usedTerrain, int oldTerrain, std::string& map);
  static void createZRmap(std::map<std::string, fs::path>& terrainOverrides,
                          fs::path outputDir, std::string mapName);
  static void terrainSwap(genie::DatFile* hdDat, genie::DatFile* aocDat,
                          int tNew, int tOld, int slpID);
  void indexDrsFiles(fs::path const& src, bool expansionFiles = true,
                     bool terrainFiles = false);
  inline void indexDrsFiles(fs::path const& src, WKSettings::IndexType flags) {
    indexDrsFiles(
        src, static_cast<int>(flags & WKSettings::IndexType::Expansion) != 0,
        static_cast<int>(flags & WKSettings::IndexType::Terrain) != 0);
  }
  static void copyHistoryFiles(fs::path inputDir, fs::path outputDir);
  static std::pair<int, std::string> parseHDTextLine(std::string line);
  static std::optional<std::pair<int, std::string>> parseHDTextLine(std::string line) const noexcept;
  void convertLanguageFile(std::ifstream& in, std::ofstream& iniOut,
                           std::map<int, std::string>& langReplacement);
  void createLanguageFile(fs::path languageIniPath, fs::path patchFolder);
  static void loadGameStrings(std::map<int, std::string>& langReplacement,
                              fs::path file);
  static void loadModdedStrings(fs::path moddedStringsFile,
                                std::map<int, std::string>& langReplacement);
  static void makeRandomMapScriptsDrs(std::ofstream& out,
                                      const fs::path& drsDir);
  void makeDrs(std::ofstream& out);
  void editDrs(std::ifstream* in, std::ofstream* out);
  static void copyCivIntroSounds(const fs::path& inputDir,
                                 const fs::path& outputDir);
  void copyWallFiles(const fs::path& inputDir);
  static void createMusicPlaylist(const fs::path& inputDir,
                                  const fs::path& outputDir);
  void transferHdDatElements(genie::DatFile* hdDat, genie::DatFile* aocDat);
  static void adjustArchitectureFlags(genie::DatFile* aocDat,
                                      fs::path flagFilename);
  void patchArchitectures(genie::DatFile* aocDat);
  bool checkGraphics(genie::DatFile* aocDat, short graphicID,
                     std::vector<int> checkedGraphics);
  void replaceGraphic(genie::DatFile* aocDat, short* graphicID, short compareID,
                      short c, std::map<short, short>& replacedGraphics,
                      bool civGroups = false);
  short duplicateGraphic(genie::DatFile* aocDat,
                         std::map<short, short>& replacedGraphics,
                         std::vector<short> duplicatedGraphics, short graphicID,
                         short compareID, short offset, bool civGroups = false);
  static bool identifyHotkeyFile(const fs::path& directory, fs::path& maxHki,
                                 fs::path& lastEditedHki);
  static void copyHotkeyFile(const fs::path& maxHki,
                             const fs::path& lastEditedHki, fs::path dst);
  void removeWkHotkeys();
  void hotkeySetup();
  static void refreshSymlink(const fs::path& oldPath, const fs::path& newPath,
                             LinkType type, bool copyOldContents = false);
  void symlinkSetup(const fs::path& oldDir, const fs::path& newDir,
                    bool dataMod = false);
  void setupFolders(fs::path xmlOutPathUP);
};
