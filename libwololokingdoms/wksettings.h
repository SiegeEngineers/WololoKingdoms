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

    WKSettings() = default;

    bool useVoobly = true;
    bool useExe = false;
    bool useBoth = false;
    bool useMonks = true;
    bool copyMaps = true;
    bool copyCustomMaps = false;
    bool restrictedCivMods = true;
    bool fixFlags = false;
    bool replaceTooltips = true;
    // Only used for information
    bool useGrid = true;
    bool useShortWalls = false;

    std::string language = "en";
    int patch = -1;
    int hotkeyChoice = 0;
    int dlcLevel = 0;
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
