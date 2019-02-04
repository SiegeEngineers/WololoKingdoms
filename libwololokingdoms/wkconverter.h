#ifndef WKCONVERTER_H
#define WKCONVERTER_H

#include <set>
#include <regex>
#include <map>
#include <vector>
#include <string>
#include <filesystem>

#include "genie/dat/DatFile.h"
#include "genie/lang/LangFile.h"
#include "wksettings.h"

#ifndef ALLOW_UNUSED
#  ifdef __GNUC__
#    define ALLOW_UNUSED __attribute__ ((unused))
#  else
#    define ALLOW_UNUSED
#  endif
#endif

namespace fs = std::filesystem;

/**
 * WK Conversion event listener, used to update the UI and to implement platform/app-specific
 * things like UserPatch installation.
 */
class WKConvertListener {
    int m_cachedProgress = 0;
public:
    /**
     * Called when conversion has finished.
     */
    virtual void finished() {}

    /**
     * Log a message.
     */
    virtual void log(ALLOW_UNUSED std::string logMessage) {}

    /**
     * Set the information line, usually describing what the converter is currently working on.
     */
    virtual void setInfo(ALLOW_UNUSED std::string info) {}

    /**
     * Report an error.
     */
    virtual void error(ALLOW_UNUSED std::exception const & err) {}

    /**
     * Report an error (message only).
     */
    virtual void error(ALLOW_UNUSED std::string message) {
      error(std::runtime_error(message));
    }

    /**
     * Create a dialog (this can usually be a noop if you're not building a UI)
     */
    virtual void createDialog(ALLOW_UNUSED std::string info) {}

    /**
     * Create a dialog (this can usually be a noop if you're not building a UI)
     */
    virtual void createDialog(ALLOW_UNUSED std::string info, ALLOW_UNUSED std::string title) {}

    /**
     * Create a dialog (this can usually be a noop if you're not building a UI)
     */
    virtual void createDialog(
        ALLOW_UNUSED std::string info,
        ALLOW_UNUSED std::string toReplace,
        ALLOW_UNUSED std::string replaceWith) {}

    /**
     * Set the current progress (0-100).
     */
    virtual void setProgress(ALLOW_UNUSED int i) {}

    /**
     * Increase the current progress.
     */
    virtual void increaseProgress(ALLOW_UNUSED int i) {
      m_cachedProgress += i;
      setProgress(m_cachedProgress);
    }

    /**
     * Install UserPatch.
     * userPatchExe is the path to the UserPatch SetupAoC.exe file.
     * cliFlags are strings that should be passed to the installer as command-line flags
     */
    virtual void installUserPatch(
        ALLOW_UNUSED fs::path userPatchExe,
        ALLOW_UNUSED std::vector<std::string> cliFlags) {}
};

class WKConverter {
public:
    WKConverter(WKSettings* settings, WKConvertListener* listener)
      :
      settings(settings),
      listener(listener) {
    }

    ~WKConverter() {}

    int run(bool retry = false);

private:

    WKSettings* settings;
    WKConvertListener* listener;
    std::set<char> civLetters;
    std::set<int> aocSlpFiles;
    std::map<int, fs::path> slpFiles;
    std::map<int, fs::path> wavFiles;
    std::map<std::string,fs::path> newTerrainFiles;
    std::vector<std::pair<int,std::string>> rmsCodeStrings;
    bool secondAttempt = false;
    bool allowRun = true;

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
        std::string const_name_pattern;
        std::string replaced_name_pattern;
        int old_terrain_id;
        int new_terrain_id;
        TerrainType terrain_type;
    };

    void copyHDMaps(const fs::path& inputDir, const fs::path& outputDir, bool replace = false);
    bool usesMultipleWaterTerrains(std::string& map, std::map<int,bool>& terrainsUsed);
    bool isTerrainUsed(int terrain, std::map<int,bool>& terrainsUsed, std::string& map, std::map<int,std::regex>& patterns);
    void upgradeTrees(int usedTerrain, int oldTerrain, std::string& map);
    void createZRmap(std::map<std::string,fs::path>& terrainOverrides, fs::path outputDir, std::string mapName);
    void terrainSwap(genie::DatFile *hdDat, genie::DatFile *aocDat, int tNew, int tOld, int slpID);
    void indexDrsFiles(fs::path const &src, bool expansionFiles = true, bool terrainFiles = false);
    void copyHistoryFiles(fs::path inputDir, fs::path outputDir);
    std::pair<int,std::string> parseHDTextLine(std::string line);
    void convertLanguageFile(std::ifstream& in, std::ofstream& iniOut, std::map<int, std::string>& langReplacement);
    void createLanguageFile(fs::path languageIniPath, fs::path patchFolder);
    void loadGameStrings(std::map<int,std::string>& langReplacement);
    void loadModdedStrings(fs::path moddedStringsFile, std::map<int, std::string>& langReplacement);
    void makeDrs(std::ofstream& out);
    void editDrs(std::ifstream *in, std::ofstream *out);
    void uglyHudHack(const fs::path&);
    void copyCivIntroSounds(const fs::path& inputDir, const fs::path& outputDir);
    void copyWallFiles(const fs::path& inputDir);
    void createMusicPlaylist(const fs::path& inputDir, const fs::path& outputDir);
    void transferHdDatElements(genie::DatFile *hdDat, genie::DatFile *aocDat);
    void adjustArchitectureFlags(genie::DatFile *aocDat, fs::path flagFilename);
    void patchArchitectures(genie::DatFile *aocDat);
    bool checkGraphics(genie::DatFile *aocDat, short graphicID, std::vector<int> checkedGraphics);
    void replaceGraphic(genie::DatFile *aocDat, short* graphicID, short compareID, short c, std::map<short,short>& replacedGraphics, bool civGroups = false);
    short duplicateGraphic(genie::DatFile *aocDat, std::map<short,short>& replacedGraphics, std::vector<short> duplicatedGraphics, short graphicID, short compareID, short offset, bool civGroups = false);
    bool identifyHotkeyFile(const fs::path& directory, fs::path& maxHki, fs::path& lastEditedHki);
    void copyHotkeyFile(const fs::path& maxHki, const fs::path& lastEditedHki, fs::path dst);
    void removeWkHotkeys();
    void hotkeySetup();
    void symlinkSetup(const fs::path& oldDir, const fs::path& newDir, bool dataMod = false);
    void setupFolders(fs::path xmlOutPathUP);
    void retryInstall();
};

#endif // WKCONVERTER_H
