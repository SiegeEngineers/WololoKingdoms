#ifndef WKSETTINGS_H
#define WKSETTINGS_H

#include <filesystem>
#include <string>
#include <map>

namespace fs = std::filesystem;

class WKSettings
{


public:
    WKSettings(bool useVoobly, bool useExe, bool useBoth, bool useMonks, bool usePw, bool useWalls, bool copyMaps, bool copyCustomMaps,
       bool restrictedCivMods, bool useNoSnow, bool fixFlags, bool replaceTooltips, bool useGrid, fs::path installDirectory,
       std::string language, int dlcLevel, int patch, int hotkeyChoice, fs::path hdPath, fs::path outPath, fs::path vooblyDir, fs::path upDir,
       std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList, std::string modName)
        : useVoobly(useVoobly), useExe(useExe), useBoth(useBoth), useMonks(useMonks), usePw(usePw), useWalls(useWalls), copyMaps(copyMaps),
          copyCustomMaps(copyCustomMaps), restrictedCivMods(restrictedCivMods), useNoSnow(useNoSnow), fixFlags(fixFlags), replaceTooltips(replaceTooltips),
          useGrid(useGrid), installDirectory(installDirectory), language(language), patch(patch), hotkeyChoice(hotkeyChoice), dlcLevel(dlcLevel),
          hdPath(hdPath), outPath(outPath), vooblyDir(vooblyDir), upDir(upDir), nfzVooblyOutPath(vooblyDir/"Player.nfz"),
          nfzUpOutPath(upDir/"Player.nfz"), dataModList(dataModList), modName(modName){}
    bool useVoobly;
    bool useExe;
    bool useBoth;
    bool useMonks;
    bool usePw;
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
    std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList;
    std::string modName;
};

#endif // WKSETTINGS_H
