#pragma once
#include <fs.h>
#include <map>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class WKSettings
{
public:
    typedef std::tuple<std::string,std::string, std::string, int, std::string> DataMod;

    enum class IndexType {
      IndexOnly = 0,
      Expansion = 1,
      Terrain = 2,
    };

    WKSettings(
        bool useVoobly = true,
        bool useExe = false,
        bool useBoth = false,
        bool useMonks = true,
        bool copyMaps = true,
        bool copyCustomMaps = false,
        bool restrictedCivMods = true,
        bool fixFlags = false,
        bool replaceTooltips = true,
        bool useGrid = true,
        std::string language = "en",
        int dlcLevel = 0,
        int patch = -1,
        int hotkeyChoice = 0,
        fs::path hdPath = fs::path(),
        fs::path outPath = fs::path(),
        fs::path vooblyDir = fs::path(),
        fs::path upDir = fs::path(),
        std::map<int, DataMod> dataModList = {},
        std::string modName = ""
    ) : useVoobly(useVoobly),
        useExe(useExe),
        useBoth(useBoth),
        useMonks(useMonks),
        copyMaps(copyMaps),
        copyCustomMaps(copyCustomMaps),
        restrictedCivMods(restrictedCivMods),
        fixFlags(fixFlags),
        replaceTooltips(replaceTooltips),
        useGrid(useGrid),
        language(language),
        patch(patch),
        hotkeyChoice(hotkeyChoice),
        dlcLevel(dlcLevel),
        hdPath(hdPath),
        outPath(outPath),
        vooblyDir(vooblyDir),
        upDir(upDir),
        nfzVooblyOutPath(vooblyDir/"Player.nfz"),
        nfzUpOutPath(upDir/"Player.nfz"),
        dataModList(dataModList),
        modName(modName)
    {
    }

    bool useVoobly;
    bool useExe;
    bool useBoth;
    bool useMonks;
    bool copyMaps;
    bool copyCustomMaps;
    bool restrictedCivMods;
    bool fixFlags;
    bool replaceTooltips;
    // Only used for information
    bool useGrid;
    bool useShortWalls;

    std::string language;
    int patch;
    int hotkeyChoice;
    int dlcLevel;
    fs::path resourceDir;
    fs::path hdPath;
    fs::path outPath;
    fs::path vooblyDir;
    fs::path upDir;
    fs::path nfzVooblyOutPath;
    fs::path nfzUpOutPath;
    std::map<int, DataMod> dataModList;
    std::string modName;

    std::vector<std::pair<fs::path, IndexType>> drsModDirectories;

    inline void addDrsResources(const fs::path& directory, IndexType type = IndexType::Expansion) {
        drsModDirectories.push_back(std::make_pair(directory, type));
    }
};

inline WKSettings::IndexType operator|(WKSettings::IndexType a, WKSettings::IndexType b) {
    return static_cast<WKSettings::IndexType>(static_cast<int>(a) | static_cast<int>(b));
}
inline WKSettings::IndexType operator&(WKSettings::IndexType a, WKSettings::IndexType b) {
    return static_cast<WKSettings::IndexType>(static_cast<int>(a) & static_cast<int>(b));
}
