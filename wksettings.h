#ifndef WKSETTINGS_H
#define WKSETTINGS_H

#include <boost/filesystem.hpp>
#include <string>
#include <map>

namespace fs = boost::filesystem;

class WKSettings
{


public:
    WKSettings(bool useVoobly, bool useExe, bool useBoth, bool useMonks, bool usePw, bool useWalls, bool copyMaps, bool copyCustomMaps,
       bool restrictedCivMods, bool useNoSnow, bool fixFlags, bool replaceTooltips, bool useGrid, fs::path installDirectory,
       std::string language, std::string version, int dlcLevel, int patch, int hotkeyChoice, fs::path HDPath, fs::path outPath, fs::path vooblyDir, fs::path upDir,
       std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList, std::string modName)
        : useVoobly(useVoobly), useExe(useExe), useBoth(useBoth), useMonks(useMonks), usePw(usePw), useWalls(useWalls), copyMaps(copyMaps),
          copyCustomMaps(copyCustomMaps), restrictedCivMods(restrictedCivMods), useNoSnow(useNoSnow), fixFlags(fixFlags), replaceTooltips(replaceTooltips),
          useGrid(useGrid), installDirectory(installDirectory), language(language), version(version), patch(patch), hotkeyChoice(hotkeyChoice), dlcLevel(dlcLevel),
          HDPath(HDPath), outPath(outPath), vooblyDir(vooblyDir), upDir(upDir), nfzVooblyOutPath(vooblyDir/"Player.nfz"),
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
    std::string version;
    int patch;
    int hotkeyChoice;
    int dlcLevel;
    fs::path HDPath;
    fs::path outPath;
    fs::path vooblyDir;
    fs::path upDir;
    fs::path nfzVooblyOutPath;
    fs::path nfzUpOutPath;
    std::map<int, std::tuple<std::string,std::string, std::string, int, std::string>> dataModList;
    std::string modName;
};

#endif // WKSETTINGS_H
