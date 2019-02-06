#ifndef WKSETTINGS_H
#define WKSETTINGS_H

#include <filesystem>
#include <string>
#include <map>

namespace fs = std::filesystem;

class WKSettings
{
public:
    typedef std::tuple<std::string,std::string, std::string, int, std::string> DataMod;

    WKSettings(
        bool useVoobly = true,
        bool useExe = false,
        bool useBoth = false,
        bool useMonks = true,
        bool useSmallTrees = true,
        bool useWalls = false,
        bool copyMaps = true,
        bool copyCustomMaps = false,
        bool restrictedCivMods = true,
        bool useNoSnow = true,
        bool fixFlags = false,
        bool replaceTooltips = true,
        bool useGrid = true,
        fs::path installDirectory = fs::path(),
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
        useSmallTrees(useSmallTrees),
        useWalls(useWalls),
        copyMaps(copyMaps),
        copyCustomMaps(copyCustomMaps),
        restrictedCivMods(restrictedCivMods),
        useNoSnow(useNoSnow),
        fixFlags(fixFlags),
        replaceTooltips(replaceTooltips),
        useGrid(useGrid),
        installDirectory(installDirectory),
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
    bool useSmallTrees;
    bool useWalls;
    bool copyMaps;
    bool copyCustomMaps;
    bool restrictedCivMods;
    bool useNoSnow;
    bool fixFlags;
    bool replaceTooltips;
    bool useGrid;
    fs::path installDirectory;
    std::string language;
    int patch;
    int hotkeyChoice;
    int dlcLevel;
    fs::path hdPath;
    fs::path outPath;
    fs::path vooblyDir;
    fs::path upDir;
    fs::path nfzVooblyOutPath;
    fs::path nfzUpOutPath;
    std::map<int, DataMod> dataModList;
    std::string modName;
};

#endif // WKSETTINGS_H
