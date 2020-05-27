#pragma once
#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "platform.h"
#include "wksettings.h"
#include <filesystem>
#include <map>
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
  virtual ~WKConvertListener() {}

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
  installUserPatch([[maybe_unused]] std::filesystem::path userPatchExe,
                   [[maybe_unused]] std::vector<std::string> cliFlags) {}
};

class WKConverter {
  WKSettings settings;
  WKConvertListener* listener;

public:
  WKConverter(WKSettings& settings, WKConvertListener* const listener)
      : settings(settings), listener(listener) {
    if (settings.resourceDir != std::filesystem::path()) {
      resourceDir = settings.resourceDir;
    }
  }

  int run();
  int retryInstall();

private:
  std::set<char> civLetters;
  std::set<int> aocSlpFiles;
  std::map<int, std::filesystem::path> slpFiles;
  std::map<int, std::filesystem::path> wavFiles;
  std::map<std::string, std::filesystem::path> newTerrainFiles;
  std::vector<std::pair<int, std::string>> rmsCodeStrings;
  bool secondAttempt = false;

  std::filesystem::path nfzUpOutPath;
  std::filesystem::path nfzVooblyOutPath;
  std::filesystem::path vooblyDir;
  std::filesystem::path upDir;
  std::filesystem::path installDir;
  std::filesystem::path resourceDir = "resources";

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

  void copyHDMaps(const std::filesystem::path& inputDir,
                  const std::filesystem::path& outputDir, bool replace = false);
  bool usesMultipleWaterTerrains(const std::string& map,
                                 std::map<int, bool>& terrainsUsed);
  bool isTerrainUsed(int terrain, std::map<int, bool>& terrainsUsed,
                     const std::string& map,
                     const std::map<int, std::regex>& patterns);
  void upgradeTrees(int usedTerrain, int oldTerrain, std::string& map);
  void
  createZRmap(std::map<std::string, std::filesystem::path>& terrainOverrides,
              std::filesystem::path outputDir, std::string mapName);
  void terrainSwap(genie::DatFile* hdDat, genie::DatFile* aocDat, int tNew,
                   int tOld, int slpID);
  void indexDrsFiles(std::filesystem::path const& src,
                     bool expansionFiles = true, bool terrainFiles = false);
  inline void indexDrsFiles(std::filesystem::path const& src,
                            WKSettings::IndexType flags) {
    indexDrsFiles(
        src, static_cast<int>(flags & WKSettings::IndexType::Expansion) != 0,
        static_cast<int>(flags & WKSettings::IndexType::Terrain) != 0);
  }
  void copyHistoryFiles(std::filesystem::path inputDir,
                        std::filesystem::path outputDir);
  std::pair<int, std::string> parseHDTextLine(std::string line);
  void convertLanguageFile(std::ifstream& in, std::ofstream& iniOut,
                           std::map<int, std::string>& langReplacement);
  void createLanguageFile(std::filesystem::path languageIniPath,
                          std::filesystem::path patchFolder);
  void loadGameStrings(std::map<int, std::string>& langReplacement,
                       std::filesystem::path file);
  void loadModdedStrings(std::filesystem::path moddedStringsFile,
                         std::map<int, std::string>& langReplacement);
  void makeRandomMapScriptsDrs(std::ofstream& out,
                               const std::filesystem::path& drsDir);
  void makeDrs(std::ofstream& out);
  void editDrs(std::ifstream* in, std::ofstream* out);
  void copyCivIntroSounds(const std::filesystem::path& inputDir,
                          const std::filesystem::path& outputDir);
  void copyWallFiles(const std::filesystem::path& inputDir);
  void createMusicPlaylist(const std::filesystem::path& inputDir,
                           const std::filesystem::path& outputDir);
  void transferHdDatElements(genie::DatFile* hdDat, genie::DatFile* aocDat);
  void adjustArchitectureFlags(genie::DatFile* aocDat,
                               std::filesystem::path flagFilename);
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
  bool identifyHotkeyFile(const std::filesystem::path& directory,
                          std::filesystem::path& maxHki,
                          std::filesystem::path& lastEditedHki);
  void copyHotkeyFile(const std::filesystem::path& maxHki,
                      const std::filesystem::path& lastEditedHki,
                      std::filesystem::path dst);
  void removeWkHotkeys();
  void hotkeySetup();
  void refreshSymlink(const std::filesystem::path& oldDir,
                      const std::filesystem::path& newDir, const LinkType type,
                      bool copyOldContents = false);
  void symlinkSetup(const std::filesystem::path& oldDir,
                    const std::filesystem::path& newDir, bool dataMod = false);
  void setupFolders(std::filesystem::path xmlOutPathUP);
};
